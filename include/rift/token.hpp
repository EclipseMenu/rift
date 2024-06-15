#pragma once

#include <string>
#include <utility>

namespace rift {

    enum class TokenType {
        Segment, // Static text segment

        // Variables
        IDENTIFIER, NUMBER, STRING,

        // Brackets
        LEFT_BRACE, RIGHT_BRACE,
        LEFT_PAREN, RIGHT_PAREN,

        // Operators
        PLUS, MINUS, STAR, SLASH, PERCENT,
        QUESTION, COLON,
        LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, NOT_EQUAL,

        // Logical operators
        AND, OR, NOT,

        // Special tokens
        DOT, COMMA,

        // Special tokens
        END, ERROR
    };

    struct Token {
        TokenType type;
        std::string value;

        Token(TokenType type, std::string  value) : type(type), value(std::move(value)) {}
    };

}