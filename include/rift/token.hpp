#pragma once
#ifndef RIFT_TOKEN_HPP
#define RIFT_TOKEN_HPP

#include <array>
#include <optional>
#include <string_view>

namespace rift {

    enum class TokenType {
        SEGMENT, // Static text segment

        // Variables
        IDENTIFIER, FLOAT, INTEGER, STRING, // x 1.0 1 "hello"

        // Brackets
        LEFT_BRACE, RIGHT_BRACE,     // { }
        LEFT_PAREN, RIGHT_PAREN,     // ( )
        LEFT_BRACKET, RIGHT_BRACKET, // [ ]

        // Operators
        PLUS, MINUS, STAR, SLASH, PERCENT, CARET,                         // + - * / % ^
        QUESTION, COLON, NULL_COALESCE, ASSIGN,                           // ? : ?? :=
        LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, NOT_EQUAL, // < > <= >= == !=
        DOLLAR,                                                           // $

        // Logical operators
        AND, OR, NOT, // && || !

        // Special tokens
        DOT, COMMA, // . ,
        END // End of file
    };

    constexpr std::array TOKEN_TYPE_NAMES = {
        "SEGMENT",

        "IDENTIFIER", "FLOAT", "INTEGER", "STRING",

        "LEFT_BRACE", "RIGHT_BRACE",
        "LEFT_PAREN", "RIGHT_PAREN",
        "LEFT_BRACKET", "RIGHT_BRACKET",

        "PLUS", "MINUS", "STAR", "SLASH", "PERCENT", "CARET",
        "QUESTION", "COLON", "NULL_COALESCE", "ASSIGN",
        "LESS", "GREATER", "LESS_EQUAL", "GREATER_EQUAL", "EQUAL_EQUAL", "NOT_EQUAL",
        "DOLLAR",

        "AND", "OR", "NOT",

        "DOT", "COMMA",
        "END"
    };

    struct Token {
        TokenType type;
        std::string value;
        size_t fromIndex, toIndex;

        constexpr Token(TokenType type, size_t index, size_t toIndex)
            : type(type), fromIndex(index), toIndex(toIndex) {}

        constexpr Token(TokenType type, std::string value, size_t fromIndex, size_t toIndex)
            : type(type), value(std::move(value)), fromIndex(fromIndex), toIndex(toIndex) {}

        constexpr Token(TokenType type, std::string value, size_t index)
            : Token(type, std::move(value), index, index + value.size()) {}

        constexpr Token(TokenType type, std::string value)
            : Token(type, std::move(value), 0, value.size()) {}

        constexpr Token(TokenType type, std::string_view value, size_t fromIndex, size_t toIndex)
            : Token(type, std::string(value), fromIndex, toIndex) {}

        constexpr Token(TokenType type, std::string_view value, size_t index)
            : Token(type, std::string(value), index, index + value.size()) {}

        constexpr Token(TokenType type, std::string_view value)
            : Token(type, std::string(value), 0, value.size()) {}

        constexpr Token(TokenType type, size_t index)
            : Token(type, index, index + 1) {}

        static constexpr Token EOFToken(size_t index) {
            return { TokenType::END, index };
        }

        constexpr const char* typeName() const {
            return TOKEN_TYPE_NAMES[static_cast<size_t>(type)];
        }

        constexpr operator bool() const {
            return type != TokenType::END;
        }
    };

}

#endif // RIFT_TOKEN_HPP