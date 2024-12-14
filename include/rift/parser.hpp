#pragma once
#ifndef RIFT_PARSER_HPP
#define RIFT_PARSER_HPP

#include "lexer.hpp"
#include "nodes/node.hpp"
#include "errors/compile.hpp"

#include <memory>
#include <stack>
#include <string>
#include <string_view>

namespace rift {

    using ParseResult = geode::Result<std::unique_ptr<Node>, CompileError>;

    class Parser {
    public:
        explicit Parser(Lexer&& lexer, bool directMode = false) noexcept
            : m_lexer(std::move(lexer)), m_directMode(directMode) {}

        /// @brief Parse the source code into an AST.
        /// @return a ParseResult containing the AST if successful, otherwise a CompileError
        ParseResult parse() noexcept;

    private:
        ParseResult parseRoot() noexcept;
        ParseResult parseExpression() noexcept;
        ParseResult parseBooleanExpression() noexcept;
        ParseResult parseComparisonExpression() noexcept;
        ParseResult parseArithmeticExpression() noexcept;
        ParseResult parseTerm() noexcept;
        ParseResult parseFactor() noexcept;
        ParseResult parseInterpolation() noexcept;
        ParseResult parseAccessor() noexcept;
        ParseResult parseCall() noexcept;
        ParseResult parseAtom() noexcept;

        LexerResult advance() noexcept {
            auto res = m_lexer.next();
            if (res.isOk()) {
                m_currentToken = res.unwrap();
            }
            return res;
        }

    private:
        Lexer m_lexer;
        Token m_currentToken = Token::EOFToken(0);
        bool m_directMode = false;
    };
}

#endif // RIFT_PARSER_HPP