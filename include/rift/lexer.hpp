#pragma once

#include "token.hpp"
#include <string>

namespace rift {

    class Lexer {
    public:
        /// @brief Construct a lexer.
        /// @param script The script to lex.
        explicit Lexer(std::string  script);

        /// @brief Get the next token.
        /// @return The next token.
        Token nextToken();

    private:
        /// @brief Check whether index is at the end of the script.
        [[nodiscard]] bool isEnd() const;

        /// @brief Get the current character.
        [[nodiscard]] char peek() const;

        /// @brief Get the next character.
        [[nodiscard]] char peekNext() const;

        /// @brief Advance the index and return the current character.
        char advance();

        /// @brief Parse a number.
        Token parseNumber();

        /// @brief Parse a string.
        Token parseString();

        /// @brief Parse an identifier.
        Token parseIdentifier();

    private:
        std::string m_script;
        int32_t m_index = 0;
        size_t m_expressionDepth = 0;
    };

}