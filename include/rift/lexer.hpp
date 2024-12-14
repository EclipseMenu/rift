#pragma once
#ifndef RIFT_LEXER_HPP
#define RIFT_LEXER_HPP

#include <string>

#include "token.hpp"
#include "util.hpp"
#include "errors/compile.hpp"

namespace rift {

    using LexerResult = geode::Result<Token, CompileError>;

    class Lexer {
    public:
        explicit Lexer(std::string_view source, bool directMode = false)
            : m_source(source), m_index(0), m_directMode(directMode) {}

        /// @brief Get the next token from the source.
        LexerResult next();

        [[nodiscard]] constexpr bool isEnd() const {
            return m_index >= m_source.size();
        }

    private:
        LexerResult nextSegment();
        LexerResult nextExpression();
        LexerResult number();
        LexerResult identifier();
        LexerResult string(char quote);

        static constexpr bool isWhitespace(char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        }

        static constexpr bool isDigit(char c) {
            return c >= '0' && c <= '9';
        }

        static constexpr bool isAlpha(char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        }

        static constexpr bool isAlphaNumeric(char c) {
            return isAlpha(c) || isDigit(c);
        }

        static constexpr char escapeChar(char c) {
            switch (c) {
                case 'n': return '\n';
                case 'r': return '\r';
                case 't': return '\t';
                case 'v': return '\v';
                case 'b': return '\b';
                case 'f': return '\f';
                case 'a': return '\a';
                case '\\': return '\\';
                case '\'': return '\'';
                case '"': return '"';
                default: return c;
            }
        }

        static std::string escapedString(std::string_view source) {
            std::string result;
            result.reserve(source.size());
            for (size_t i = 0; i < source.size(); i++) {
                if (source[i] == '\\') {
                    if (i + 1 < source.size()) {
                        result.push_back(escapeChar(source[++i]));
                    }
                } else {
                    result.push_back(source[i]);
                }
            }
            return result;
        }

        [[nodiscard]] constexpr char peek() const {
            return isEnd() ? '\0' : m_source[m_index];
        }

        [[nodiscard]] constexpr char peekNext() const {
            return m_index + 1 >= m_source.size() ? '\0' : m_source[m_index + 1];
        }

        constexpr char advance() {
            return isEnd() ? '\0' : m_source[m_index++];
        }

    private:
        std::string_view m_source;
        size_t m_index;
        size_t m_expressionDepth = 0;
        bool m_directMode;

        friend class Parser; // Parser needs access to m_index for error reporting
    };

}

#endif // RIFT_LEXER_HPP