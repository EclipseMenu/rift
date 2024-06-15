#include <rift.hpp>
#include <rift/lexer.hpp>
#include <rift/visitor.hpp>

#include <rift/nodes/segment.hpp>
#include <rift/nodes/identifier.hpp>

#include <iostream>

namespace rift {

    std::string Script::run() {
        Visitor visitor(this);
        return visitor.evaluate();
    }

    const char* tokenTypeToString(TokenType type) {
        switch (type) {
            case TokenType::Segment: return "Segment";
            case TokenType::IDENTIFIER: return "Identifier";
            case TokenType::NUMBER: return "Number";
            case TokenType::STRING: return "String";
            case TokenType::LEFT_BRACE: return "Left Brace";
            case TokenType::RIGHT_BRACE: return "Right Brace";
            case TokenType::LEFT_PAREN: return "Left Paren";
            case TokenType::RIGHT_PAREN: return "Right Paren";
            case TokenType::PLUS: return "Plus";
            case TokenType::MINUS: return "Minus";
            case TokenType::STAR: return "Star";
            case TokenType::SLASH: return "Slash";
            case TokenType::PERCENT: return "Percent";
            case TokenType::QUESTION: return "Question";
            case TokenType::COLON: return "Colon";
            case TokenType::LESS: return "Less";
            case TokenType::GREATER: return "Greater";
            case TokenType::LESS_EQUAL: return "Less Equal";
            case TokenType::GREATER_EQUAL: return "Greater Equal";
            case TokenType::EQUAL_EQUAL: return "Equal Equal";
            case TokenType::NOT_EQUAL: return "Not Equal";
            case TokenType::AND: return "And";
            case TokenType::OR: return "Or";
            case TokenType::NOT: return "Not";
            case TokenType::DOT: return "Dot";
            case TokenType::COMMA: return "Comma";
            case TokenType::END: return "End";
            case TokenType::ERROR: return "Error";
        }

        return "Unknown";
    }

    Script* compile(const std::string& script) {
        Lexer lexer(script);

        auto* s = new Script();

        while (true) {
            auto token = lexer.nextToken();
            if (token.type == TokenType::END) {
                break;
            }

            if (token.type == TokenType::ERROR) {
                std::cout << "Error: " << token.value << std::endl;
                delete s;
                return nullptr;
            }

            if (token.type == TokenType::Segment) {
                s->m_nodes.push_back(new SegmentNode(token.value));
            } else if (token.type == TokenType::IDENTIFIER) {
                s->m_nodes.push_back(new IdentifierNode(token.value));
            } else {
                // std::cout << tokenTypeToString(token.type) << ": " << token.value << std::endl;
            }
        }

        return s;
    }

    std::string format(const std::string& script, const std::unordered_map<std::string, Value>& variables) {
        auto* s = compile(script);
        if (!s) {
            delete s;
            return "";
        }

        for (const auto& [name, value] : variables) {
            s->setVariable(name, value);
        }

        auto result = s->run();
        delete s;

        return result;
    }



}