#pragma once
#ifndef RIFT_ROOT_NODE_HPP
#define RIFT_ROOT_NODE_HPP

#include "node.hpp"

#include <vector>
#include <memory>

namespace rift {

    class RootNode final : public Node {
    public:
        explicit RootNode(std::vector<std::unique_ptr<Node>> nodes, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_nodes(std::move(nodes)) { m_type = Type::Root; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            std::string result = "RootNode(";
            for (size_t i = 0; i < m_nodes.size(); ++i) {
                result += m_nodes[i]->toDebugString();
                if (i + 1 < m_nodes.size()) result += ", ";
            }
            return result + ")";
        }

        [[nodiscard]] std::vector<std::unique_ptr<Node>> const& nodes() const noexcept {
            return m_nodes;
        }

    private:
        std::vector<std::unique_ptr<Node>> m_nodes;
    };

}

#endif // RIFT_ROOT_NODE_HPP