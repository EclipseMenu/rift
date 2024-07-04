#include <rift/parser.hpp>

#include <rift/nodes/binaryop.hpp>
#include <rift/nodes/functioncall.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/segment.hpp>
#include <rift/nodes/ternary.hpp>
#include <rift/nodes/unaryop.hpp>
#include <rift/nodes/value.hpp>

#include <iostream>

namespace rift {

    RootNode* Parser::parse() {
        auto* root = new RootNode;
        advance();
        while (!isAtEnd()) {
            auto block = parseBlock();
            if (block == nullptr) {
                delete root;
                return nullptr;
            }
            root->addChild(block);
        }

        return root;
    }

    Node* Parser::parseBlock() {
        if (m_currentToken.type == TokenType::LEFT_BRACE) {
            advance();
            auto *expression = parseExpression();
            if (expression == nullptr) {
                return nullptr;
            }
            if (m_currentToken.type != TokenType::RIGHT_BRACE) {
                m_error = "Expected '}'";
                return nullptr;
            }
            advance();
            return expression;
        } else {
            auto *segment = new SegmentNode(m_currentToken.value);
            advance();
            return segment;
        }
    }

    Node* Parser::parseExpression() {
        auto *expression = parseTernaryOp();
        if (expression == nullptr) {
            return parseComparisonExpression();
        }
        return expression;
    }

    Node* Parser::parseTernaryOp() {
        auto *condition = parseComparisonExpression();
        if (condition == nullptr) {
            return nullptr;
        }
        if (m_currentToken.type != TokenType::QUESTION) {
            return condition;
        }
        advance();
        auto *trueExpression = parseExpression();
        if (trueExpression == nullptr) {
            return nullptr;
        }
        if (m_currentToken.type != TokenType::COLON) {
            m_error = "Expected ':'";
            return nullptr;
        }
        advance();
        auto *falseExpression = parseExpression();
        if (falseExpression == nullptr) {
            return nullptr;
        }
        return new TernaryNode(condition, trueExpression, falseExpression);
    }

    Node* Parser::parseComparisonExpression() {
        auto *expression = parseArithmeticExpression();
        if (expression == nullptr) {
            return nullptr;
        }
        while (m_currentToken.type == TokenType::EQUAL_EQUAL || m_currentToken.type == TokenType::NOT_EQUAL ||
               m_currentToken.type == TokenType::LESS || m_currentToken.type == TokenType::LESS_EQUAL ||
               m_currentToken.type == TokenType::GREATER || m_currentToken.type == TokenType::GREATER_EQUAL) {
            auto type = m_currentToken.type;
            advance();
            auto *right = parseArithmeticExpression();
            if (right == nullptr) {
                return nullptr;
            }
            expression = new BinaryOpNode(expression, right, type);
        }
        return expression;
    }

    Node* Parser::parseArithmeticExpression() {
        auto *expression = parseTerm();
        if (expression == nullptr) {
            return nullptr;
        }
        while (m_currentToken.type == TokenType::PLUS || m_currentToken.type == TokenType::MINUS) {
            auto type = m_currentToken.type;
            advance();
            auto *right = parseTerm();
            if (right == nullptr) {
                return nullptr;
            }
            expression = new BinaryOpNode(expression, right, type);
        }
        return expression;
    }

    Node* Parser::parseTerm() {
        auto *expression = parseFactor();
        if (expression == nullptr) {
            return nullptr;
        }
        while (m_currentToken.type == TokenType::STAR || m_currentToken.type == TokenType::SLASH ||
               m_currentToken.type == TokenType::PERCENT) {
            auto type = m_currentToken.type;
            advance();
            auto *right = parseFactor();
            if (right == nullptr) {
                return nullptr;
            }
            expression = new BinaryOpNode(expression, right, type);
        }
        return expression;
    }

    Node* Parser::parseFactor() {
        if (m_currentToken.type == TokenType::PLUS || m_currentToken.type == TokenType::MINUS) {
            auto type = m_currentToken.type;
            advance();
            auto *factor = parseFactor();
            if (factor == nullptr) {
                return nullptr;
            }
            return new UnaryOpNode(type, factor);
        }
        return parsePower();
    }

    Node* Parser::parsePower() {
        auto *expression = parseCall();
        if (expression == nullptr) {
            return nullptr;
        }
        while (m_currentToken.type == TokenType::CARET) {
            advance();
            auto *right = parseFactor();
            if (right == nullptr) {
                return nullptr;
            }
            expression = new BinaryOpNode(expression, right, TokenType::CARET);
        }
        return expression;
    }

    Node* Parser::parseCall() {
        auto *expression = parseAtom();
        if (expression == nullptr) {
            return nullptr;
        }
        if (m_currentToken.type != TokenType::LEFT_PAREN) {
            return expression;
        }
        advance();
        std::vector<Node*> arguments;
        while (m_currentToken.type != TokenType::RIGHT_PAREN) {
            auto *argument = parseExpression();
            if (argument == nullptr) {
                return nullptr;
            }
            arguments.push_back(argument);
            if (m_currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        }
        if (m_currentToken.type != TokenType::RIGHT_PAREN) {
            m_error = "Expected ')'";
            return nullptr;
        }
        advance();
        return new FunctionCallNode(expression, arguments);
    }

    Node* Parser::parseAtom() {
        if (m_currentToken.type == TokenType::FLOAT) {
            auto *value = new ValueNode(Value::from(std::stof(m_currentToken.value)));
            advance();
            return value;
        } else if (m_currentToken.type == TokenType::INTEGER) {
            auto *value = new ValueNode(Value::from(std::stoi(m_currentToken.value)));
            advance();
            return value;
        } else if (m_currentToken.type == TokenType::STRING) {
            auto *value = new ValueNode(Value::from(m_currentToken.value.substr(1, m_currentToken.value.size() - 2)));
            advance();
            return value;
        } else if (m_currentToken.type == TokenType::IDENTIFIER) {
            auto *identifier = new IdentifierNode(m_currentToken.value);
            advance();
            return identifier;
        } else if (m_currentToken.type == TokenType::LEFT_PAREN) {
            advance();
            auto *expression = parseExpression();
            if (expression == nullptr) {
                return nullptr;
            }
            if (m_currentToken.type != TokenType::RIGHT_PAREN) {
                m_error = "Expected ')'";
                return nullptr;
            }
            advance();
            return expression;
        } else {
            m_error = "Expected number, string, identifier, or '('";
            return nullptr;
        }
    }

    Token Parser::advance() {
        auto token = m_currentToken;
        m_currentToken = m_lexer.nextToken();
        return token;
    }

}