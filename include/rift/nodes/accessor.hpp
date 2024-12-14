#pragma once
#ifndef RIFT_ACCESSOR_NODE_HPP
#define RIFT_ACCESSOR_NODE_HPP

#include "node.hpp"

#include <memory>
#include <string>

namespace rift {

    class AccessorNode final : public Node {
    public:
        explicit AccessorNode(std::unique_ptr<Node> node, std::string name, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_node(std::move(node)), m_name(std::move(name)) { m_type = Type::Accessor; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("AccessorNode({}, {})", m_node->toDebugString(), m_name);
        }

        [[nodiscard]] std::unique_ptr<Node> const& node() const noexcept {
            return m_node;
        }

        [[nodiscard]] std::string const& name() const noexcept {
            return m_name;
        }

    private:
        std::unique_ptr<Node> m_node;
        std::string m_name;
    };

}

#endif // RIFT_ACCESSOR_NODE_HPP