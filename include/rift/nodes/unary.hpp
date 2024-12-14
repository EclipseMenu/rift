#pragma once
#ifndef RIFT_UNARY_NODE_HPP
#define RIFT_UNARY_NODE_HPP

#include "node.hpp"

namespace rift {

    class UnaryNode final : public Node {
    public:
        explicit UnaryNode(TokenType op, std::unique_ptr<Node> value, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_value(std::move(value)), m_op(op) { m_type = Type::Unary; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("UnaryNode({}, {})", TOKEN_TYPE_NAMES[static_cast<size_t>(m_op)], m_value->toDebugString());
        }

        [[nodiscard]] TokenType op() const noexcept {
            return m_op;
        }

        [[nodiscard]] Node const& value() const noexcept {
            return *m_value;
        }

    private:
        std::unique_ptr<Node> m_value;
        TokenType m_op;
    };

}

#endif // RIFT_UNARY_NODE_HPP