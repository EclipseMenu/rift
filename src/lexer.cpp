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

    inline char escapeChar(char c) {
        switch (c) {
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'a': return '\a';
            default: return c;
        }
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
            }
            if (isAlpha(c)) {
                return parseIdentifier();
            }
            switch (c) {
                case '\'': case '"': return parseString(c);
                case '(': return createToken(TokenType::LEFT_PAREN, "(");
                case ')': return createToken(TokenType::RIGHT_PAREN, ")");
                case '+': return createToken(TokenType::PLUS, "+");
                case '-': return createToken(TokenType::MINUS, "-");
                case '*': return createToken(TokenType::STAR, "*");
                case '/': return createToken(TokenType::SLASH, "/");
                case '%': return createToken(TokenType::PERCENT, "%");
                case '^': return createToken(TokenType::CARET, "^");
                case ':': {
                    if (peek() == '=') {
                        advance();
                        return createToken(TokenType::ASSIGN, ":=");
                    }
                    return createToken(TokenType::COLON, ":");
                }
                case '?': {
                    if (peek() == '?') {
                        advance();
                        return createToken(TokenType::NULL_COALESCE, "??");
                    }
                    return createToken(TokenType::QUESTION, "?");
                }
                case '<': {
                    if (peek() == '=') {
                        advance();
                        return createToken(TokenType::LESS_EQUAL, "<=");
                    }
                    return createToken(TokenType::LESS, "<");
                }
                case '>': {
                    if (peek() == '=') {
                        advance();
                        return createToken(TokenType::GREATER_EQUAL, ">=");
                    }
                    return createToken(TokenType::GREATER, ">");
                }
                case '=': {
                    if (peek() == '=') {
                        advance();
                        return createToken(TokenType::EQUAL_EQUAL, "==");
                    }
                    return createToken(TokenType::ERROR, "expected double equals sign");
                }
                case '!': {
                    if (peek() == '=') {
                        advance();
                        return createToken(TokenType::NOT_EQUAL, "!=");
                    }
                    return createToken(TokenType::NOT, "!");
                }
                case '&': {
                    if (peek() == '&') {
                        advance();
                        return createToken(TokenType::AND, "&&");
                    }
                    return createToken(TokenType::ERROR, "expected double ampersand");
                }
                case '|': {
                    if (peek() == '|') {
                        advance();
                        return createToken(TokenType::OR, "||");
                    }
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

        std::string buffer;
        // Read until the next expression.
        while (!isEnd()) {
            char c = peek();
            if (c == '{') {
                if (buffer.empty()) {
                    advance();
                    m_expressionDepth++;
                    return createToken(TokenType::LEFT_BRACE, "{");
                }
                break;
            }
            buffer.push_back(c);
            advance();
        }

        // If we have a buffer, return it as a string.
        std::string newBuffer;
        newBuffer.reserve(buffer.size());
        bool escape = false;
        for (char c : buffer) {
            if (c == '\\') {
                escape = true;
                continue;
            }
            if (escape) {
                newBuffer.push_back(escapeChar(c));
                escape = false;
            } else {
                newBuffer.push_back(c);
            }
        }
        return createToken(TokenType::Segment, newBuffer);
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

    bool Lexer::readString(std::string& output, char readUntil, bool requireEnd) {
        char c = advance();
        while (c && c != readUntil) {
            if (c == '\\') {
                c = advance();
                if (!c) return false;
                output.push_back(escapeChar(c));
            } else {
                output.push_back(c);
            }
            c = advance();
            if (!c) return false;
        }

        return !requireEnd || c == readUntil;
    }

    Token Lexer::parseString(char quote) {
        std::string buffer;
        if (!readString(buffer, quote)) {
            return createToken(TokenType::ERROR, "unterminated string");
        }
        return createToken(TokenType::STRING, buffer);
    }

    Token Lexer::parseIdentifier() {
        size_t start = m_index - 1;

        while (isAlphaNumeric(peek()) || peek() == '_') advance();

        return createToken(TokenType::IDENTIFIER, m_script.substr(start, m_index - start));
    }

}
