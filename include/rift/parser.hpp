#pragma once

#include <utility>

#include "nodes/root.hpp"
#include "result.hpp"
#include "lexer.hpp"

namespace rift {

    /* Parser syntax:
     *
     * root                  : block*
     * block                 : segment | '{' expression '}'
     *
     * expression            : ternary_op
     *                       : comparison_expression
     *
     * ternary_op            : comparison_expression '?' expression ':' expression
     *
     * comparison_expression : '!' comparison_expression
     *                       : arithmetic_expression ('==' | '!=' | '<' | '<=' | '>' | '>=' arithmetic_expression)*
     *
     * arithmetic_expression : term (('+' | '-') term)*
     * term                  : factor (('*' | '/' | '%') factor)*
     * factor                : ('+' | '-') factor
     *                       : power
     *
     * power                 : call ('^' factor)*
     *
     * call                  : atom ('(' (expression (',' expression)*)? ')')?
     *
     * atom                  : number | string | identifier
     *                       : '(' expression ')'
     *
     */

    /// @brief A parser for the RIFT syntax, which converts a list of tokens into an AST.
    class Parser {
    public:
        /// @brief Construct the parser.
        /// @param tokens The tokens to parse.
        explicit Parser(Lexer lexer) : m_lexer(lexer) {}

        /// @brief Parse the tokens into an AST.
        /// @return Result object containing the root node.
        Result<RootNode*> parse();

        /// @brief Check if the parser is at the end of the tokens.
        [[nodiscard]] bool isAtEnd() const { return m_currentToken.type == TokenType::END; }

    private:
        /// @brief Advance to the next token.
        /// @return Previous token.
        inline Token advance();

        // Parse functions

        Result<Node*> parseBlock();
        Result<Node*> parseExpression();
        Result<Node*> parseTernaryOp();
        Result<Node*> parseComparisonExpression();
        Result<Node*> parseArithmeticExpression();
        Result<Node*> parseTerm();
        Result<Node*> parseFactor();
        Result<Node*> parsePower();
        Result<Node*> parseCall();
        Result<Node*> parseAtom();

        [[nodiscard]] std::string getErrorMessage(std::string_view message) const;

    private:
        Lexer m_lexer;
        Token m_currentToken = { TokenType::END, "", 0, 0 };
        size_t m_index = 0;
    };

}