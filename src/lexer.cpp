#include <rift/lexer.hpp>
#include <utility>

namespace rift {

    Lexer::Lexer(std::string script)
        : m_script(std::move(script)) {}

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
        if (isEnd()) {
            return '\0';
        }

        return m_script[m_index];
    }

    char Lexer::peekNext() const {
        if (m_index + 1 >= m_script.size()) {
            return '\0';
        }

        return m_script[m_index + 1];
    }

    char Lexer::advance() {
        if (isEnd()) {
            return '\0';
        }

        return m_script[m_index++];
    }

    Token Lexer::nextToken() {
        size_t start = m_index;

        if (isEnd()) {
            return Token{TokenType::END, ""};
        }

        if (m_expressionDepth > 0) {
            // We are inside an expression, so we need to parse it.

            // Skip whitespace.
            while (true) {
                if (isEnd()) return Token{TokenType::END, ""};
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
                    case '\'':
                    case '"':
                        return parseString();
                        break;

                    case '(': return Token{TokenType::LEFT_PAREN, "("};
                    case ')': return Token{TokenType::RIGHT_PAREN, ")"};
                    case '+': return Token{TokenType::PLUS, "+"};
                    case '-': return Token{TokenType::MINUS, "-"};
                    case '*': return Token{TokenType::STAR, "*"};
                    case '/': return Token{TokenType::SLASH, "/"};
                    case '%': return Token{TokenType::PERCENT, "%"};
                    case '^': return Token{TokenType::CARET, "^"};
                    case '?': return Token{TokenType::QUESTION, "?"};
                    case ':': return Token{TokenType::COLON, ":"};
                    case '<':
                        if (peek() == '=') {
                            advance();
                            return Token{TokenType::LESS_EQUAL, "<="};
                        } else {
                            return Token{TokenType::LESS, "<"};
                        }
                    case '>':
                        if (peek() == '=') {
                            advance();
                            return Token{TokenType::GREATER_EQUAL, ">="};
                        } else {
                            return Token{TokenType::GREATER, ">"};
                        }
                    case '=':
                        if (peek() == '=') {
                            advance();
                            return Token{TokenType::EQUAL_EQUAL, "=="};
                        } else {
                            return Token{TokenType::ERROR, "expected double equals sign"};
                        }
                    case '!':
                        if (peek() == '=') {
                            advance();
                            return Token{TokenType::NOT_EQUAL, "!="};
                        } else {
                            return Token{TokenType::NOT, "!"};
                        }
                    case '&':
                        if (peek() == '&') {
                            advance();
                            return Token{TokenType::AND, "&&"};
                        } else {
                            return Token{TokenType::ERROR, "expected double ampersand"};
                        }
                    case '|':
                        if (peek() == '|') {
                            advance();
                            return Token{TokenType::OR, "||"};
                        } else {
                            return Token{TokenType::ERROR, "expected double pipe"};
                        }
                    case '.': return Token{TokenType::DOT, "."};
                    case ',': return Token{TokenType::COMMA, ","};
                    case '{':
                        m_expressionDepth++;
                        return Token{TokenType::LEFT_BRACE, "{"};
                    case '}':
                        m_expressionDepth--;
                        return Token{TokenType::RIGHT_BRACE, "}"};
                    default:
                        return Token{TokenType::ERROR, "unexpected character '" + std::string(1, c) + "'"};
                }
            }
        } else {
            // Read a segment of static text.
            char c = advance();

            if (c == '{') {
                // We are entering an expression.
                m_expressionDepth++;
                return Token{TokenType::LEFT_BRACE, "{"};
            }
            else {
                while (!isEnd() && peek() != '{' && peekNext() != '{') {
                    c = advance();

                    if (c == '{' && peek() == '{') {
                        advance();
                    }
                }

                return Token{TokenType::Segment, m_script.substr(start, m_index - start)};
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
            return Token{TokenType::FLOAT, m_script.substr(start, m_index - start)};
        }

        return Token{TokenType::INTEGER, m_script.substr(start, m_index - start)};
    }

    Token Lexer::parseString() {
        size_t start = m_index - 1;
        while (!isEnd() && peek() != '\'' && peek() != '"') {
            advance();
        }

        if (isEnd()) {
            return Token{TokenType::ERROR, "unterminated string"};
        }

        advance(); // Consume the closing quote.
        return Token{TokenType::STRING, m_script.substr(start, m_index - start)};
    }

    Token Lexer::parseIdentifier() {
        size_t start = m_index - 1;

        while (isAlphaNumeric(peek())) advance();

        return Token{TokenType::IDENTIFIER, m_script.substr(start, m_index - start)};
    }

}