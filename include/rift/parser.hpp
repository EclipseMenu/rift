#pragma once

#include <utility>

#include "lexer.hpp"
#include "rift/nodes/root.hpp"

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
        explicit Parser(Lexer lexer) : m_lexer(std::move(lexer)) {}

        /// @brief Parse the tokens into an AST.
        /// @return The AST.
        RootNode* parse();

        /// @brief Check if the parser is at the end of the tokens.
        [[nodiscard]] bool isAtEnd() const { return m_currentToken.type == TokenType::END; }

        /// @brief Get the error message.
        [[nodiscard]] const std::string& getError() const { return m_error; }

    private:
        /// @brief Advance to the next token.
        /// @return Previous token.
        Token advance();

        // Parse functions

        Node* parseBlock();
        Node* parseExpression();
        Node* parseTernaryOp();
        Node* parseComparisonExpression();
        Node* parseArithmeticExpression();
        Node* parseTerm();
        Node* parseFactor();
        Node* parsePower();
        Node* parseCall();
        Node* parseAtom();

    private:
        Lexer m_lexer;
        Token m_currentToken = { TokenType::END, "" };
        std::string m_error;
    };

}