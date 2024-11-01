#pragma once

#include <string>
#include <utility>
#include <memory>

namespace rift {

    enum class TokenType {
        Segment, // Static text segment

        // Variables
        IDENTIFIER, FLOAT, INTEGER, STRING,

        // Brackets
        LEFT_BRACE, RIGHT_BRACE,
        LEFT_PAREN, RIGHT_PAREN,

        // Operators
        PLUS, MINUS, STAR, SLASH, PERCENT, CARET,
        QUESTION, COLON, NULL_COALESCE, ASSIGN,
        LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, NOT_EQUAL,
        DOLLAR,

        // Logical operators
        AND, OR, NOT,

        // Special tokens
        DOT, COMMA,

        // Internal tokens
        END, ERROR
    };

    inline const char* const TOKEN_NAMES[] = {
        "Segment",
        "Identifier", "Float", "Integer", "String",
        "Left Brace", "Right Brace",
        "Left Paren", "Right Paren",
        "Plus", "Minus", "Star", "Slash", "Percent", "Caret",
        "Question", "Colon",
        "Less", "Greater", "Less Equal", "Greater Equal", "Equal Equal", "Not Equal",
        "And", "Or", "Not",
        "Dot", "Comma",
        "End", "Error"
    };

    inline const char* tokenTypeToString(TokenType type) {
        return TOKEN_NAMES[static_cast<int>(type)];
    }

    struct Token {
        TokenType type;
        std::string value;
        size_t startIndex;
        size_t endIndex;

        Token(TokenType type, std::string value, size_t startIndex, size_t endIndex)
            : type(type), value(std::move(value)), startIndex(startIndex), endIndex(endIndex) {}
    };

}