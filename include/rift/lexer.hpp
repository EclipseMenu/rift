#pragma once

#include "token.hpp"
#include <string>

namespace rift {

    class Lexer {
    public:
        /// @brief Construct a lexer.
        /// @param script The script to lex.
        explicit Lexer(std::string_view script);

        /// @brief Get the next token.
        /// @return The next token.
        Token nextToken();

        /// @brief Check whether index is at the end of the script.
        [[nodiscard]] inline bool isEnd() const {
            return m_index >= m_script.size();
        }

    private:
        /// @brief Get the current character.
        [[nodiscard]] inline char peek() const {
            return isEnd() ? '\0' : m_script[m_index];
        }

        /// @brief Get the next character.
        [[nodiscard]] inline char peekNext() const {
            return (m_index + 1 >= m_script.size()) ? '\0' : m_script[m_index + 1];
        }

        /// @brief Advance the index and return the current character.
        inline char advance() {
            return isEnd() ? '\0' : m_script[m_index++];
        }

        /// @brief Parse a number.
        Token parseNumber();

        /// @brief Parse a string.
        Token parseString();

        /// @brief Parse an identifier.
        Token parseIdentifier();

        /// @brief Create a token with current index.
        [[nodiscard]] inline Token createToken(TokenType type, std::string_view value) const {
            return Token{type, value, static_cast<size_t>(m_startIndex), value.size()};
        }

    private:
        std::string_view m_script;
        int32_t m_index = 0;
        int32_t m_startIndex = 0;
        size_t m_expressionDepth = 0;
    };

}