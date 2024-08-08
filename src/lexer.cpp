#include <rift/lexer.hpp>
#include <utility>
#include <fmt/format.h>

namespace rift {

    Lexer::Lexer(std::string_view script)
        : m_script(script) {}

    inline bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    inline bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    inline bool isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    bool Lexer::isEnd() const {
        return m_index >= m_script.size();
    }

    char Lexer::peek() const {
        return isEnd() ? '\0' : m_script[m_index];
    }

    char Lexer::peekNext() const {
        return (m_index + 1 >= m_script.size()) ? '\0' : m_script[m_index + 1];
    }

    char Lexer::advance() {
        return isEnd() ? '\0' : m_script[m_index++];
    }

    Token Lexer::createToken(TokenType type, std::string_view value) const {
        return Token{type, value, static_cast<size_t>(m_startIndex), value.size()};
    }

    Token Lexer::nextToken() {
        m_startIndex = m_index;

        if (isEnd()) {
            return createToken(TokenType::END, "");
        }

        if (m_expressionDepth > 0) {
            // We are inside an expression, so we need to parse it.

            // Skip whitespace.
            while (true) {
                if (isEnd()) return createToken(TokenType::END, "");
                char c = peek();
                if (!isWhitespace(c)) break;
                advance();
            }

            char c = advance();
            if (isDigit(c) || (c == '-' && isDigit(peek()))) {
                return parseNumber();
            } else if (isAlpha(c)) {
                return parseIdentifier();
            } else {
                switch (c) {
                    case '\'': case '"': return parseString();
                    case '(': return createToken(TokenType::LEFT_PAREN, "(");
                    case ')': return createToken(TokenType::RIGHT_PAREN, ")");
                    case '+': return createToken(TokenType::PLUS, "+");
                    case '-': return createToken(TokenType::MINUS, "-");
                    case '*': return createToken(TokenType::STAR, "*");
                    case '/': return createToken(TokenType::SLASH, "/");
                    case '%': return createToken(TokenType::PERCENT, "%");
                    case '^': return createToken(TokenType::CARET, "^");
                    case '?': return createToken(TokenType::QUESTION, "?");
                    case ':': return createToken(TokenType::COLON, ":");
                    case '<':
                        if (peek() == '=') {
                            advance();
                            return createToken(TokenType::LESS_EQUAL, "<=");
                        } else {
                            return createToken(TokenType::LESS, "<");
                        }
                    case '>':
                        if (peek() == '=') {
                            advance();
                            return createToken(TokenType::GREATER_EQUAL, ">=");
                        } else {
                            return createToken(TokenType::GREATER, ">");
                        }
                    case '=':
                        if (peek() == '=') {
                            advance();
                            return createToken(TokenType::EQUAL_EQUAL, "==");
                        } else {
                            return createToken(TokenType::ERROR, "expected double equals sign");
                        }
                    case '!':
                        if (peek() == '=') {
                            advance();
                            return createToken(TokenType::NOT_EQUAL, "!=");
                        } else {
                            return createToken(TokenType::NOT, "!");
                        }
                    case '&':
                        if (peek() == '&') {
                            advance();
                            return createToken(TokenType::AND, "&&");
                        } else {
                            return createToken(TokenType::ERROR, "expected double ampersand");
                        }
                    case '|':
                        if (peek() == '|') {
                            advance();
                            return createToken(TokenType::OR, "||");
                        } else {
                            return createToken(TokenType::ERROR, "expected double pipe");
                        }
                    case '.': return createToken(TokenType::DOT, ".");
                    case ',': return createToken(TokenType::COMMA, ",");
                    case '{':
                        m_expressionDepth++;
                        return createToken(TokenType::LEFT_BRACE, "{");
                    case '}':
                        m_expressionDepth--;
                        return createToken(TokenType::RIGHT_BRACE, "}");
                    default:
                        return createToken(TokenType::ERROR, fmt::format("unexpected character '{}'", c));
                }
            }
        } else {
            // Read a segment of static text.
            char c = advance();

            if (c == '{') {
                // We are entering an expression.
                m_expressionDepth++;
                return createToken(TokenType::LEFT_BRACE, "{");
            }
            else {
                while (!isEnd() && peek() != '{' && peekNext() != '{') {
                    c = advance();

                    if (c == '{' && peek() == '{') {
                        advance();
                    }
                }

                return createToken(TokenType::Segment, m_script.substr(m_startIndex, m_index - m_startIndex));
            }
        }
    }

    Token Lexer::parseNumber() {
        size_t start = m_index - 1;

        // Parse the integer part.
        while (isDigit(peek())) advance();

        // Parse the fractional part.
        bool hasDot = false;
        if (peek() == '.' && isDigit(peekNext())) {
            hasDot = true;
            advance(); // Consume the dot.
            while (isDigit(peek())) advance();
        }

        if (hasDot) {
            return createToken(TokenType::FLOAT, m_script.substr(start, m_index - start));
        }

        return createToken(TokenType::INTEGER, m_script.substr(start, m_index - start));
    }

    Token Lexer::parseString() {
        size_t start = m_index - 1;
        while (!isEnd() && peek() != '\'' && peek() != '"') {
            advance();
        }

        if (isEnd()) {
            return createToken(TokenType::ERROR, "unterminated string");
        }

        advance(); // Consume the closing quote.
        return createToken(TokenType::STRING, m_script.substr(start, m_index - start));
    }

    Token Lexer::parseIdentifier() {
        size_t start = m_index - 1;

        while (isAlphaNumeric(peek()) || peek() == '_') advance();

        return createToken(TokenType::IDENTIFIER, m_script.substr(start, m_index - start));
    }

}