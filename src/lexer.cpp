#include <rift/lexer.hpp>

namespace rift {

    LexerResult Lexer::next() {
        if (isEnd()) {
            return geode::Ok(Token::EOFToken(m_index));
        }

        if (m_directMode || m_expressionDepth > 0) {
            return nextExpression();
        }

        return nextSegment();
    }

    LexerResult Lexer::nextSegment() {
        size_t start = m_index;
        while (!isEnd()) {
            if (peek() == '{') {
                if (m_index == start) {
                    m_expressionDepth++;
                    return geode::Ok(Token { TokenType::LEFT_BRACE, m_index, ++m_index });
                }
                break;
            }
            m_index++;
        }

        return geode::Ok(Token { TokenType::SEGMENT, escapedString(m_source.substr(start, m_index - start)), start });
    }

    LexerResult Lexer::nextExpression() {
        // skip whitespace
        while (!isEnd() && isWhitespace(peek())) {
            m_index++;
        }

        // check for end of expression
        if (isEnd()) {
            if (m_directMode) {
                return geode::Ok(Token::EOFToken(m_index));
            }

            return geode::Err(CompileError(
                std::string(m_source),
                fmt::format("LexerError: Unexpected EOF at index {}", m_index),
                m_index - 1,
                m_source.size()
            ));
        }

        char c = advance();
        if (isDigit(c)) {
            return number();
        }
        if (isAlpha(c) || c == '_') {
            return identifier();
        }

        switch (c) {
            case '\'': case '"': return string(c);
            case '(': return geode::Ok(Token { TokenType::LEFT_PAREN, m_index - 1 });
            case ')': return geode::Ok(Token { TokenType::RIGHT_PAREN, m_index - 1 });
            case '[': return geode::Ok(Token { TokenType::LEFT_BRACKET, m_index - 1 });
            case ']': return geode::Ok(Token { TokenType::RIGHT_BRACKET, m_index - 1 });
            case '+': return geode::Ok(Token { TokenType::PLUS, m_index - 1 });
            case '-': return geode::Ok(Token { TokenType::MINUS, m_index - 1 });
            case '*': return geode::Ok(Token { TokenType::STAR, m_index - 1 });
            case '/': return geode::Ok(Token { TokenType::SLASH, m_index - 1 });
            case '%': return geode::Ok(Token { TokenType::PERCENT, m_index - 1 });
            case '^': return geode::Ok(Token { TokenType::CARET, m_index - 1 });
            case '$': return geode::Ok(Token { TokenType::DOLLAR, m_index - 1 });
            case '.': return geode::Ok(Token { TokenType::DOT, m_index - 1 });
            case ',': return geode::Ok(Token { TokenType::COMMA, m_index - 1 });
            case '?': {
                if (peek() == '?') {
                    m_index++;
                    return geode::Ok(Token { TokenType::NULL_COALESCE, m_index - 2 });
                }
                return geode::Ok(Token { TokenType::QUESTION, m_index - 1 });
            }
            case ':': {
                if (peek() == '=') {
                    m_index++;
                    return geode::Ok(Token { TokenType::ASSIGN, m_index - 2 });
                }
                return geode::Ok(Token { TokenType::COLON, m_index - 1 });
            }
            case '<': {
                if (peek() == '=') {
                    m_index++;
                    return geode::Ok(Token { TokenType::LESS_EQUAL, m_index - 2 });
                }
                return geode::Ok(Token { TokenType::LESS, m_index - 1 });
            }
            case '>': {
                if (peek() == '=') {
                    m_index++;
                    return geode::Ok(Token { TokenType::GREATER_EQUAL, m_index - 2 });
                }
                return geode::Ok(Token { TokenType::GREATER, m_index - 1 });
            }
            case '=': {
                if (peek() == '=') {
                    m_index++;
                    return geode::Ok(Token { TokenType::EQUAL_EQUAL, m_index - 2 });
                }
                return geode::Err(CompileError(
                    std::string(m_source),
                    fmt::format("LexerError: Unexpected '=' at index {}", m_index - 1),
                    m_index - 1,
                    m_index
                ));
            }
            case '!': {
                if (peek() == '=') {
                    m_index++;
                    return geode::Ok(Token { TokenType::NOT_EQUAL, m_index - 2 });
                }
                return geode::Ok(Token { TokenType::NOT, m_index - 1 });
            }
            case '&': {
                if (peek() == '&') {
                    m_index++;
                    return geode::Ok(Token { TokenType::AND, m_index - 2 });
                }
                return geode::Err(CompileError(
                    std::string(m_source),
                    fmt::format("LexerError: Unexpected '&' at index {}", m_index - 1),
                    m_index - 1,
                    m_index
                ));
            }
            case '|': {
                if (peek() == '|') {
                    m_index++;
                    return geode::Ok(Token { TokenType::OR, m_index - 2 });
                }
                return geode::Err(CompileError(
                    std::string(m_source),
                    fmt::format("LexerError: Unexpected '|' at index {}", m_index - 1),
                    m_index - 1,
                    m_index
                ));
            }
            case '{': {
                m_expressionDepth++;
                return geode::Ok(Token { TokenType::LEFT_BRACE, m_index - 1 });
            }
            case '}': {
                if (m_expressionDepth == 0) {
                    return geode::Err(CompileError(
                        std::string(m_source),
                        fmt::format("LexerError: Unexpected '}}' at index {}", m_index - 1),
                        m_index - 1,
                        m_index
                    ));
                }
                m_expressionDepth--;
                return geode::Ok(Token { TokenType::RIGHT_BRACE, m_index - 1 });
            }
            default:
                return geode::Err(CompileError(
                    std::string(m_source),
                    fmt::format("LexerError: Unexpected character '{}' at index {}", c, m_index - 1),
                    m_index - 1,
                    m_index
                ));
        }
    }

    LexerResult Lexer::number() {
        size_t start = m_index - 1;
        while (isDigit(peek())) {
            m_index++;
        }

        bool isFloat = false;
        if (peek() == '.' && isDigit(peekNext())) {
            isFloat = true;
            m_index++;
            while (isDigit(peek())) {
                m_index++;
            }
        }

        if (isFloat) {
            return geode::Ok(Token { TokenType::FLOAT, m_source.substr(start, m_index - start), start });
        }

        return geode::Ok(Token { TokenType::INTEGER, m_source.substr(start, m_index - start), start });
    }

    LexerResult Lexer::identifier() {
        size_t start = m_index - 1;
        while (isAlphaNumeric(peek()) || peek() == '_') {
            m_index++;
        }
        return geode::Ok(Token { TokenType::IDENTIFIER, m_source.substr(start, m_index - start), start });
    }

    LexerResult Lexer::string(char quote) {
        size_t start = m_index;
        while (!isEnd() && peek() != quote) {
            if (peek() == '\\') {
                m_index++;
                if (isEnd()) {
                    return geode::Err(CompileError(
                        std::string(m_source),
                        fmt::format("LexerError: Unexpected EOF in string at index {}", m_index),
                        m_index - 1,
                        m_index
                    ));
                }
            }

            m_index++;
            if (isEnd()) {
                return geode::Err(CompileError(
                    std::string(m_source),
                    fmt::format("LexerError: Unexpected EOF in string at index {}", m_index),
                    m_index - 1,
                    m_index
                ));
            }
        }

        return geode::Ok(Token { TokenType::STRING, escapedString(m_source.substr(start, m_index++ - start)), start });
    }
}
