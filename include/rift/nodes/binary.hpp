#pragma once
#ifndef RIFT_BINARY_NODE_HPP
#define RIFT_BINARY_NODE_HPP

#include "node.hpp"
#include "../token.hpp"

#include <memory>

namespace rift {

    class BinaryNode final : public Node {
    public:
        explicit BinaryNode(std::unique_ptr<Node> lhs, TokenType op, std::unique_ptr<Node> rhs, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)), m_op(op) { m_type = Type::Binary; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("BinaryNode({}, {}, {})", m_lhs->toDebugString(), TOKEN_TYPE_NAMES[static_cast<size_t>(m_op)], m_rhs->toDebugString());
        }

        [[nodiscard]] std::unique_ptr<Node> const& lhs() const noexcept { return m_lhs; }
        [[nodiscard]] std::unique_ptr<Node> const& rhs() const noexcept { return m_rhs; }
        [[nodiscard]] TokenType op() const noexcept { return m_op; }

    private:
        std::unique_ptr<Node> m_lhs, m_rhs;
        TokenType m_op;
    };

}

#endif // RIFT_BINARY_NODE_HPP