#include <rift/parser.hpp>

#include <rift/nodes/accessor.hpp>
#include <rift/nodes/binary.hpp>
#include <rift/nodes/call.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/indexer.hpp>
#include <rift/nodes/root.hpp>
#include <rift/nodes/segment.hpp>
#include <rift/nodes/ternary.hpp>
#include <rift/nodes/unary.hpp>
#include <rift/nodes/value.hpp>

namespace rift {

    /** Parser syntax:
     *
     *  root_direct             : expression
     *  root                    : (segment | LEFT_BRACE expression RIGHT_BRACE)*
     *
     *  expression              : ternary_expression
     *                          : boolean_expression
     *
     *  ternary_expression      : boolean_expression QUESTION expression COLON expression
     *                          : boolean_expression NULL_COALESCE expression
     *
     *  boolean_expression      : comparison_expression ((AND | OR) comparison_expression)?
     *
     *  comparison_expression   : arithmetic_expression ((EQUAL_EQUAL | NOT_EQUAL | LESS | GREATER | LESS_EQUAL | GREATER_EQUAL) arithmetic_expression)?
     *
     *  arithmetic_expression   : term ((PLUS | MINUS) term)*
     *  term                    : factor ((STAR | SLASH | PERCENT) factor)*
     *  factor                  : interpolation (CARET factor)?
     *                          : (PLUS | MINUS | NOT) factor
     *
     *  interpolation           : DOLLAR accessor
     *
     *  accessor                : accessor (DOT IDENTIFIER)*
     *                          : accessor (LEFT_BRACKET expression RIGHT_BRACKET)*
     *                          : call
     *
     *  call                    : atom (LEFT_PAREN (expression (COMMA expression)*)? RIGHT_PAREN)?
     *
     *  atom                    : IDENTIFIER | FLOAT | INTEGER | STRING
     *                          : LEFT_PAREN expression RIGHT_PAREN
     *
     **/

#define UNWRAP_ADVANCE() \
    auto GEODE_CONCAT(res_, __LINE__) = advance(); \
    if (GEODE_CONCAT(res_, __LINE__).isErr()) { \
        return geode::Err(CompileError( \
            std::string(m_lexer.m_source), \
            std::string(GEODE_CONCAT(res_, __LINE__).unwrapErr().message()), \
            GEODE_CONCAT(res_, __LINE__).unwrapErr().index(), \
            m_lexer.m_source.size() \
        )); \
    }

#define UNWRAP_NEXT_TOKEN(name) \
    UNWRAP_ADVANCE() \
    name = GEODE_CONCAT(res_, __LINE__).unwrap();

#define EXPECT_TOKEN(expected) \
    if (m_currentToken.type != expected) { \
        return geode::Err(CompileError( \
            std::string(m_lexer.m_source), \
            fmt::format("ParseError: Expected '{}' but found '{}' at index {}", TOKEN_TYPE_NAMES[static_cast<size_t>(expected)], TOKEN_TYPE_NAMES[static_cast<size_t>(m_currentToken.type)], m_currentToken.fromIndex), \
            m_currentToken.fromIndex, \
            m_currentToken.toIndex \
        )); \
    }

#define CONSUME_TOKEN(expected) \
    EXPECT_TOKEN(expected) \
    UNWRAP_ADVANCE()

#define UNWRAP_CONSUME_TOKEN(name, expected) \
    EXPECT_TOKEN(expected) \
    UNWRAP_NEXT_TOKEN(name)

    ParseResult Parser::parse() noexcept {
        UNWRAP_ADVANCE() // Get the first token
        if (m_directMode) {
            return parseExpression();
        }
        return parseRoot();
    }

    ParseResult Parser::parseRoot() noexcept {
        std::vector<std::unique_ptr<Node>> nodes;
        while (m_currentToken) {
            switch (m_currentToken.type) {
                case TokenType::SEGMENT: {
                    nodes.push_back(std::make_unique<SegmentNode>(m_currentToken));
                    UNWRAP_ADVANCE()
                } break;
                case TokenType::LEFT_BRACE: {
                    UNWRAP_ADVANCE() // consume the left brace
                    auto res = parseExpression();
                    if (res.isErr()) {
                        return res;
                    }
                    nodes.push_back(std::move(res.unwrap()));
                    CONSUME_TOKEN(TokenType::RIGHT_BRACE);
                } break;
                default: {
                    return geode::Err(CompileError(
                        std::string(m_lexer.m_source),
                        fmt::format("ParseError: Unexpected token '{}' at index {}", m_currentToken.typeName(), m_currentToken.fromIndex),
                        m_currentToken.fromIndex,
                        m_currentToken.toIndex
                    ));
                }
            }
        }

        // if we only have one node, return it directly
        if (nodes.size() == 1) {
            return geode::Ok(std::move(nodes[0]));
        }

        // otherwise, create a root node with all the segments
        return geode::Ok(std::make_unique<RootNode>(std::move(nodes), 0, m_lexer.m_source.size()));
    }

    ParseResult Parser::parseExpression() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseBooleanExpression();
        if (res.isErr()) {
            return res;
        }

        // check if we have a ternary expression
        switch (m_currentToken.type) {
            case TokenType::QUESTION: {
                UNWRAP_ADVANCE()
                auto trueBranch = parseExpression();
                if (trueBranch.isErr()) {
                    return trueBranch;
                }

                CONSUME_TOKEN(TokenType::COLON)
                auto falseBranch = parseExpression();
                if (falseBranch.isErr()) {
                    return falseBranch;
                }

                return geode::Ok(std::make_unique<TernaryNode>(
                    std::move(res.unwrap()),
                    std::move(trueBranch.unwrap()),
                    std::move(falseBranch.unwrap()),
                    start, m_currentToken.toIndex
                ));
            }
            case TokenType::NULL_COALESCE: {
                UNWRAP_ADVANCE()
                auto trueBranch = parseExpression();
                if (trueBranch.isErr()) {
                    return trueBranch;
                }

                return geode::Ok(std::make_unique<TernaryNode>(
                    std::move(res.unwrap()),
                    std::move(trueBranch.unwrap()),
                    start, m_currentToken.toIndex
                ));
            }

            // otherwise, return the boolean expression
            default: {
                return res;
            }
        }
    }

    ParseResult Parser::parseBooleanExpression() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseComparisonExpression();
        if (res.isErr()) {
            return res;
        }

        switch (m_currentToken.type) {
            case TokenType::AND:
            case TokenType::OR: {
                auto op = m_currentToken;
                UNWRAP_ADVANCE()
                auto rhs = parseComparisonExpression();
                if (rhs.isErr()) {
                    return rhs;
                }

                return geode::Ok(std::make_unique<BinaryNode>(
                    std::move(res.unwrap()),
                    op.type,
                    std::move(rhs.unwrap()),
                    start, m_currentToken.toIndex
                ));
            }

            default: {
                return res;
            }
        }
    }

    ParseResult Parser::parseComparisonExpression() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseArithmeticExpression();
        if (res.isErr()) {
            return res;
        }

        switch (m_currentToken.type) {
            case TokenType::EQUAL_EQUAL:
            case TokenType::NOT_EQUAL:
            case TokenType::LESS:
            case TokenType::GREATER:
            case TokenType::LESS_EQUAL:
            case TokenType::GREATER_EQUAL: {
                auto op = m_currentToken;
                UNWRAP_ADVANCE()
                auto rhs = parseArithmeticExpression();
                if (rhs.isErr()) {
                    return rhs;
                }

                return geode::Ok(std::make_unique<BinaryNode>(
                    std::move(res.unwrap()),
                    op.type,
                    std::move(rhs.unwrap()),
                    start, m_currentToken.toIndex
                ));
            }

            default: {
                return res;
            }
        }

    }

    ParseResult Parser::parseArithmeticExpression() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseTerm();
        if (res.isErr()) {
            return res;
        }

        std::stack<std::unique_ptr<Node>> terms;
        terms.push(std::move(res.unwrap()));

        while (m_currentToken.type == TokenType::PLUS || m_currentToken.type == TokenType::MINUS) {
            auto type = m_currentToken.type;
            UNWRAP_ADVANCE()
            auto rhs = parseTerm();
            if (rhs.isErr()) {
                return rhs;
            }

            auto lhs = std::move(terms.top());
            terms.pop();
            terms.push(std::make_unique<BinaryNode>(
                std::move(lhs),
                type,
                std::move(rhs.unwrap()),
                start, m_currentToken.toIndex
            ));
        }

        if (terms.size() == 1) {
            return geode::Ok(std::move(terms.top()));
        }

        return geode::Err(CompileError(
            std::string(m_lexer.m_source),
            fmt::format("ParseError: Expected arithmetic expression but found '{}' at index {}", m_currentToken.typeName(), m_currentToken.fromIndex),
            m_currentToken.fromIndex,
            m_currentToken.toIndex
        ));
    }

    ParseResult Parser::parseTerm() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseFactor();
        if (res.isErr()) {
            return res;
        }

        std::stack<std::unique_ptr<Node>> terms;
        terms.push(std::move(res.unwrap()));

        while (m_currentToken.type == TokenType::STAR || m_currentToken.type == TokenType::SLASH || m_currentToken.type == TokenType::PERCENT) {
            auto type = m_currentToken.type;
            UNWRAP_ADVANCE()
            auto rhs = parseFactor();
            if (rhs.isErr()) {
                return rhs;
            }

            auto lhs = std::move(terms.top());
            terms.pop();
            terms.push(std::make_unique<BinaryNode>(
                std::move(lhs),
                type,
                std::move(rhs.unwrap()),
                start, m_currentToken.toIndex
            ));
        }

        if (terms.size() == 1) {
            return geode::Ok(std::move(terms.top()));
        }

        return geode::Err(CompileError(
            std::string(m_lexer.m_source),
            fmt::format("ParseError: Expected term but found '{}' at index {}", m_currentToken.typeName(), m_currentToken.fromIndex),
            m_currentToken.fromIndex,
            m_currentToken.toIndex
        ));
    }

    ParseResult Parser::parseFactor() noexcept {
        size_t start = m_currentToken.fromIndex;

        // check for unary operators
        switch (m_currentToken.type) {
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::NOT: {
                auto op = m_currentToken;
                UNWRAP_ADVANCE()
                auto res = parseFactor();
                if (res.isErr()) {
                    return res;
                }

                return geode::Ok(std::make_unique<UnaryNode>(
                    op.type,
                    std::move(res.unwrap()),
                    start, m_currentToken.toIndex
                ));
            }

            default: break;
        }

        auto res = parseInterpolation();
        if (res.isErr()) {
            return res;
        }

        if (m_currentToken.type == TokenType::CARET) {
            UNWRAP_ADVANCE()
            auto rhs = parseFactor();
            if (rhs.isErr()) {
                return rhs;
            }

            return geode::Ok(std::make_unique<BinaryNode>(
                std::move(res.unwrap()),
                TokenType::CARET,
                std::move(rhs.unwrap()),
                start, m_currentToken.toIndex
            ));
        }

        return res;
    }

    ParseResult Parser::parseInterpolation() noexcept {
        size_t start = m_currentToken.fromIndex;
        if (m_currentToken.type != TokenType::DOLLAR) {
            return parseAccessor();
        }

        UNWRAP_ADVANCE()
        auto res = parseAccessor();
        if (res.isErr()) {
            return res;
        }

        return geode::Ok(std::make_unique<UnaryNode>(
            TokenType::DOLLAR,
            std::move(res.unwrap()),
            start, m_currentToken.toIndex
        ));
    }

    ParseResult Parser::parseAccessor() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseCall();
        if (res.isErr()) {
            return res;
        }

        std::stack<std::unique_ptr<Node>> accessors;
        accessors.push(std::move(res.unwrap()));

        while (m_currentToken.type == TokenType::DOT || m_currentToken.type == TokenType::LEFT_BRACKET) {
            switch (m_currentToken.type) {
                case TokenType::DOT: {
                    UNWRAP_NEXT_TOKEN(auto key)
                    CONSUME_TOKEN(TokenType::IDENTIFIER)
                    accessors.push(std::make_unique<AccessorNode>(
                        std::move(accessors.top()),
                        key.value,
                        start, m_currentToken.toIndex
                    ));
                } break;

                case TokenType::LEFT_BRACKET: {
                    UNWRAP_ADVANCE()
                    auto key = parseExpression();
                    if (key.isErr()) {
                        return key;
                    }

                    CONSUME_TOKEN(TokenType::RIGHT_BRACKET)
                    accessors.push(std::make_unique<IndexerNode>(
                        std::move(accessors.top()),
                        std::move(key.unwrap()),
                        start, m_currentToken.toIndex
                    ));
                } break;

                default: {
                    break;
                }
            }
        }

        return geode::Ok(std::move(accessors.top()));
    }

    ParseResult Parser::parseCall() noexcept {
        size_t start = m_currentToken.fromIndex;
        auto res = parseAtom();
        if (res.isErr()) {
            return res;
        }

        if (m_currentToken.type != TokenType::LEFT_PAREN) {
            return res;
        }

        UNWRAP_ADVANCE()
        std::vector<std::unique_ptr<Node>> args;
        while (m_currentToken.type != TokenType::RIGHT_PAREN) {
            auto arg = parseExpression();
            if (arg.isErr()) {
                return arg;
            }

            args.push_back(std::move(arg.unwrap()));

            if (m_currentToken.type != TokenType::COMMA) {
                break;
            }

            UNWRAP_ADVANCE()
        }
        CONSUME_TOKEN(TokenType::RIGHT_PAREN)

        return geode::Ok(std::make_unique<CallNode>(
            std::move(res.unwrap()),
            std::move(args),
            start, m_currentToken.toIndex
        ));
    }

    ParseResult Parser::parseAtom() noexcept {
        switch (m_currentToken.type) {
            case TokenType::IDENTIFIER: {
                auto name = m_currentToken;
                UNWRAP_ADVANCE()
                return geode::Ok(std::make_unique<IdentifierNode>(name));
            }

            case TokenType::STRING: {
                auto node = std::make_unique<ValueNode>(
                    Value(m_currentToken.value),
                    m_currentToken.fromIndex,
                    m_currentToken.toIndex
                );
                UNWRAP_ADVANCE()
                return geode::Ok(std::move(node));
            }

            case TokenType::INTEGER: {
                auto number = util::readNumber<int64_t>(m_currentToken.value);
                if (number.isErr()) {
                    return geode::Err(CompileError(
                        std::string(m_lexer.m_source),
                        fmt::format("ParseError: Invalid integer '{}' at index {}", m_currentToken.value, m_currentToken.fromIndex),
                        m_currentToken.fromIndex,
                        m_currentToken.toIndex
                    ));
                }
                auto node = std::make_unique<ValueNode>(
                    Value(number.unwrap()),
                    m_currentToken.fromIndex,
                    m_currentToken.toIndex
                );
                UNWRAP_ADVANCE()
                return geode::Ok(std::move(node));
            }

            case TokenType::FLOAT: {
                auto number = util::readNumber<double>(m_currentToken.value);
                if (number.isErr()) {
                    return geode::Err(CompileError(
                        std::string(m_lexer.m_source),
                        fmt::format("ParseError: Invalid float '{}' at index {}", m_currentToken.value, m_currentToken.fromIndex),
                        m_currentToken.fromIndex,
                        m_currentToken.toIndex
                    ));
                }
                auto node = std::make_unique<ValueNode>(
                    Value(number.unwrap()),
                    m_currentToken.fromIndex,
                    m_currentToken.toIndex
                );
                UNWRAP_ADVANCE()
                return geode::Ok(std::move(node));
            }

            case TokenType::LEFT_PAREN: {
                UNWRAP_ADVANCE()
                auto res = parseExpression();
                if (res.isErr()) {
                    return res;
                }

                CONSUME_TOKEN(TokenType::RIGHT_PAREN)
                return res;
            }

            default: {
                return geode::Err(CompileError(
                    std::string(m_lexer.m_source),
                    fmt::format("ParseError: Unexpected token '{}' at index {}", m_currentToken.typeName(), m_currentToken.fromIndex),
                    m_currentToken.fromIndex,
                    m_currentToken.toIndex
                ));
            }
        }
    }
}
