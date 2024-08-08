#include <rift/parser.hpp>

#include <rift/nodes/binaryop.hpp>
#include <rift/nodes/functioncall.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/segment.hpp>
#include <rift/nodes/ternary.hpp>
#include <rift/nodes/unaryop.hpp>
#include <rift/nodes/value.hpp>

#include <iostream>
#include <format>

namespace rift {

    std::string Parser::getErrorMessage(std::string_view message) const {
        return std::format("{} at index {}", message, m_index);
    }

    Result<RootNode*> Parser::parse() {
        auto* root = new RootNode;
        advance();
        while (!isAtEnd()) {
            auto res = parseBlock();
            if (!res) {
                delete root;
                return Result<RootNode*>::error(res.getMessage());
            }
            root->addChild(res.getValue());
        }

        return Result<RootNode*>::success(root);
    }

    Result<Node*> Parser::parseBlock() {
        if (m_currentToken.type == TokenType::LEFT_BRACE) {
            advance();
            auto expressionRes = parseExpression();
            if (!expressionRes) {
                return expressionRes;
            }
            if (m_currentToken.type != TokenType::RIGHT_BRACE) {
                return Result<Node*>::error(getErrorMessage("Expected '}'"));
            }
            advance();
            return expressionRes;
        } else {
            auto* segment = new SegmentNode(m_currentToken.value);
            advance();
            return Result<Node*>::success(segment);
        }
    }

    Result<Node*> Parser::parseExpression() {
        auto expression = parseTernaryOp();
        if (!expression) {
            return parseComparisonExpression();
        }
        return expression;
    }

    Result<Node*> Parser::parseTernaryOp() {
        auto condition = parseComparisonExpression();
        if (!condition || m_currentToken.type != TokenType::QUESTION) {
            return condition;
        }
        advance();
        auto trueExpression = parseExpression();
        if (!trueExpression) {
            delete condition.getValue();
            return trueExpression;
        }
        if (m_currentToken.type != TokenType::COLON) {
            return Result<Node*>::error(getErrorMessage("Expected ':'"));
        }
        advance();
        auto falseExpression = parseExpression();
        if (!falseExpression) {
            delete trueExpression.getValue();
            delete condition.getValue();
            return falseExpression;
        }
        auto* ternary = new TernaryNode(condition.getValue(), trueExpression.getValue(), falseExpression.getValue());
        return Result<Node*>::success(ternary);
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
            expression = new BinaryOpNode(expression, right.getValue(), type);
        }
        return Result<Node*>::success(expression);
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
            expression = new BinaryOpNode(expression, right.getValue(), type);
        }
        return Result<Node*>::success(expression);
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
            expression = new BinaryOpNode(expression, right.getValue(), type);
        }
        return Result<Node*>::success(expression);
    }

    Result<Node*> Parser::parseFactor() {
        switch (m_currentToken.type) {
            case TokenType::PLUS:
            case TokenType::MINUS: {
                auto type = m_currentToken.type;
                advance();
                auto factor = parseFactor();
                if (!factor) return factor;
                auto *unary = new UnaryOpNode(type, factor.getValue());
                return Result<Node *>::success(unary);
            }
            default:
                return parsePower();
        }
    }

    Result<Node*> Parser::parsePower() {
        auto res = parseCall();
        if (!res) return res;
        auto* expression = res.getValue();
        while (m_currentToken.type == TokenType::CARET) {
            advance();
            auto right = parseFactor();
            if (!right) {
                delete expression;
                return right;
            }
            expression = new BinaryOpNode(expression, right.getValue(), TokenType::CARET);
        }
        return Result<Node*>::success(expression);
    }

    Result<Node*> Parser::parseCall() {
        auto res = parseAtom();
        if (!res || m_currentToken.type != TokenType::LEFT_PAREN) {
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
            arguments.push_back(argument.getValue());
            if (m_currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        }
        if (m_currentToken.type != TokenType::RIGHT_PAREN) {
            delete expression;
            for (auto arg : arguments)
                delete arg;
            return Result<Node*>::error(getErrorMessage("Expected ')'"));
        }
        advance();
        auto* functionCall = new FunctionCallNode(expression, arguments);
        return Result<Node*>::success(functionCall);
    }

    float readFloat(std::string_view str) {
        return std::stof(std::string(str));
    }

    int readInt(std::string_view str) {
        return std::stoi(std::string(str));
    }

    Result<Node*> Parser::parseAtom() {
        Node* atom;
        switch (m_currentToken.type) {
            case TokenType::FLOAT:
                atom = new ValueNode(Value::from(readFloat(m_currentToken.value)));
                advance();
                break;
            case TokenType::INTEGER:
                atom = new ValueNode(Value::from(readInt(m_currentToken.value)));
                advance();
                break;
            case TokenType::STRING:
                atom = new ValueNode(Value::string(m_currentToken.value.substr(1, m_currentToken.value.size() - 2)));
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
                    return Result<Node*>::error(getErrorMessage("Expected ')'"));
                }
                atom = expression.getValue();
                advance();
            } break;
            default:
                return Result<Node*>::error(getErrorMessage("Expected number, string, identifier, or '('"));
        }
        return Result<Node*>::success(atom);
    }

    Token Parser::advance() {
        auto token = m_currentToken;
        m_currentToken = m_lexer.nextToken();
        m_index = m_currentToken.startIndex + 1;
        return token;
    }

}