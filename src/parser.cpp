#include <rift/parser.hpp>

#include <rift/nodes/binaryop.hpp>
#include <rift/nodes/execute.hpp>
#include <rift/nodes/functioncall.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/segment.hpp>
#include <rift/nodes/ternary.hpp>
#include <rift/nodes/unaryop.hpp>
#include <rift/nodes/value.hpp>

#include <iostream>
#include <fmt/format.h>

namespace rift {

    std::string Parser::getErrorMessage(std::string_view message) const {
        return fmt::format("{} at index {}", message, m_index);
    }

    Result<RootNode*> Parser::parse() {
        auto* root = new RootNode;
        advance();
        while (!isAtEnd()) {
            auto res = parseBlock();
            if (!res) {
                delete root;
                return Err(res.getMessage());
            }
            root->addChild(res.getValue());
        }

        return root;
    }

    Result<Node*> Parser::parseBlock() {
        if (m_currentToken.type == TokenType::LEFT_BRACE) {
            advance();
            auto expressionRes = parseExpression();
            if (!expressionRes) {
                return expressionRes;
            }
            if (m_currentToken.type != TokenType::RIGHT_BRACE) {
                return Err(getErrorMessage("Expected '}'"));
            }
            advance();
            return expressionRes;
        }

        auto* segment = new SegmentNode(m_currentToken.value);
        advance();
        return segment;
    }

    Result<Node*> Parser::parseTernaryOp() {
        auto condition = parseBooleanMath();
        auto ternaryMode = m_currentToken.type;
        if ((ternaryMode != TokenType::QUESTION && ternaryMode != TokenType::NULL_COALESCE) || !condition) {
            return condition;
        }
        advance();
        auto trueExpression = parseExpression();
        if (!trueExpression) {
            delete condition.getValue();
            return trueExpression;
        }

        if (ternaryMode == TokenType::NULL_COALESCE) {
            return new TernaryNode(*condition, *trueExpression);
        }

        if (m_currentToken.type != TokenType::COLON) {
            return Err(getErrorMessage("Expected ':'"));
        }
        advance();
        auto falseExpression = parseExpression();
        if (!falseExpression) {
            delete trueExpression.getValue();
            delete condition.getValue();
            return falseExpression;
        }
        return new TernaryNode(*condition, *trueExpression, *falseExpression);
    }

    Result<Node*> Parser::parseBooleanMath() {
        auto res = parseComparisonExpression();
        if (!res) {
            return res;
        }
        auto* expression = res.getValue();
        while (m_currentToken.type == TokenType::AND || m_currentToken.type == TokenType::OR) {
            auto type = m_currentToken.type;
            advance();
            auto right = parseComparisonExpression();
            if (!right) {
                delete expression;
                return right;
            }
            expression = new BinaryOpNode(expression, right.getValue(), type);
        }
        return expression;
    }

    Result<Node*> Parser::parseComparisonExpression() {
        auto res = parseArithmeticExpression();
        if (!res) {
            return res;
        }
        auto* expression = res.getValue();
        while (m_currentToken.type == TokenType::EQUAL_EQUAL || m_currentToken.type == TokenType::NOT_EQUAL ||
               m_currentToken.type == TokenType::LESS || m_currentToken.type == TokenType::LESS_EQUAL ||
               m_currentToken.type == TokenType::GREATER || m_currentToken.type == TokenType::GREATER_EQUAL) {
            auto type = m_currentToken.type;
            advance();
            auto right = parseArithmeticExpression();
            if (!right) {
                delete expression;
                return right;
            }
            expression = new BinaryOpNode(expression, *right, type);
        }
        return expression;
    }

    Result<Node*> Parser::parseArithmeticExpression() {
        auto res = parseTerm();
        if (!res) return res;
        auto* expression = res.getValue();
        while (m_currentToken.type == TokenType::PLUS || m_currentToken.type == TokenType::MINUS) {
            auto type = m_currentToken.type;
            advance();
            auto right = parseTerm();
            if (!right) {
                delete expression;
                return right;
            }
            expression = new BinaryOpNode(expression, *right, type);
        }
        return expression;
    }

    Result<Node*> Parser::parseTerm() {
        auto res = parseFactor();
        if (!res) return res;
        auto* expression = res.getValue();
        while (m_currentToken.type == TokenType::STAR || m_currentToken.type == TokenType::SLASH ||
               m_currentToken.type == TokenType::PERCENT) {
            auto type = m_currentToken.type;
            advance();
            auto right = parseFactor();
            if (!right) {
                delete expression;
                return right;
            }
            expression = new BinaryOpNode(expression, *right, type);
        }
        return expression;
    }

    Result<Node*> Parser::parseFactor() {
        switch (m_currentToken.type) {
            case TokenType::NOT:
            case TokenType::PLUS:
            case TokenType::MINUS: {
                auto type = m_currentToken.type;
                advance();
                auto factor = parseFactor();
                if (!factor) return factor;
                return new UnaryOpNode(type, *factor);
            }
            default:
                return parsePower();
        }
    }

    Result<Node*> Parser::parsePower() {
        auto res = parseInterpolate();
        if (!res) return res;
        auto* expression = res.getValue();
        while (m_currentToken.type == TokenType::CARET) {
            advance();
            auto right = parseFactor();
            if (!right) {
                delete expression;
                return right;
            }
            expression = new BinaryOpNode(expression, *right, TokenType::CARET);
        }
        return expression;
    }

    Result<Node*> Parser::parseInterpolate() {
        if (m_currentToken.type != TokenType::DOLLAR) {
            return parseCall();
        }
        advance();
        auto res = parseCall();
        if (!res) {
            return res;
        }
        return new ExecuteNode(*res);
    }

    Result<Node*> Parser::parseCall() {
        auto res = parseAtom();
        if (m_currentToken.type != TokenType::LEFT_PAREN || !res) {
            return res;
        }
        advance();
        auto* expression = res.getValue();
        std::vector<Node*> arguments;
        while (m_currentToken.type != TokenType::RIGHT_PAREN) {
            auto argument = parseExpression();
            if (!argument) {
                delete expression;
                for (auto arg : arguments) {
                    delete arg;
                }
                return argument;
            }
            arguments.push_back(*argument);
            if (m_currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        }
        if (m_currentToken.type != TokenType::RIGHT_PAREN) {
            delete expression;
            for (auto arg : arguments)
                delete arg;
            return Err(getErrorMessage("Expected ')'"));
        }
        advance();
        return new FunctionCallNode(expression, arguments);
    }

    Result<Node*> Parser::parseAtom() {
        Node* atom;
        switch (m_currentToken.type) {
            case TokenType::FLOAT:
                atom = new ValueNode(Value::from(util::readNumber<float>(m_currentToken.value).unwrapOr(0.0f)));
                advance();
                break;
            case TokenType::INTEGER:
                atom = new ValueNode(Value::from(util::readNumber<int>(m_currentToken.value).unwrapOr(0)));
                advance();
                break;
            case TokenType::STRING:
                atom = new ValueNode(Value::string(m_currentToken.value));
                advance();
                break;
            case TokenType::IDENTIFIER:
                atom = new IdentifierNode(m_currentToken.value);
                advance();
                break;
            case TokenType::LEFT_PAREN: {
                advance();
                auto expression = parseExpression();
                if (!expression) {
                    return expression;
                }
                if (m_currentToken.type != TokenType::RIGHT_PAREN) {
                    delete expression.getValue();
                    return Err(getErrorMessage("Expected ')'"));
                }
                atom = expression.getValue();
                advance();
            } break;
            default:
                return Err(getErrorMessage("Expected number, string, identifier, or '('"));
        }
        return atom;
    }

    Token Parser::advance() {
        auto token = m_currentToken;
        m_currentToken = m_lexer.nextToken();
        m_index = m_currentToken.startIndex + 1;
        return token;
    }

}