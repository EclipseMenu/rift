#pragma once
#ifndef RIFT_IDENTIFIER_NODE_HPP
#define RIFT_IDENTIFIER_NODE_HPP

#include "node.hpp"

namespace rift {

    class IdentifierNode final : public Node {
    public:
        explicit IdentifierNode(std::string name, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_name(std::move(name)) { m_type = Type::Identifier; }

        explicit IdentifierNode(Token const& token) noexcept
            : Node(token.fromIndex, token.toIndex), m_name(token.value) { m_type = Type::Identifier; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("IdentifierNode({})", m_name);
        }

        [[nodiscard]] std::string const& name() const noexcept {
            return m_name;
        }

    private:
        std::string m_name;
    };

}

#endif // RIFT_IDENTIFIER_NODE_HPP