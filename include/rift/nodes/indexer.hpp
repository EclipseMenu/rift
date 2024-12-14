#pragma once
#ifndef RIFT_INDEXER_NODE_HPP
#define RIFT_INDEXER_NODE_HPP

#include "node.hpp"

namespace rift {

    class IndexerNode final : public Node {
    public:
        explicit IndexerNode(std::unique_ptr<Node> node, std::unique_ptr<Node> index, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_node(std::move(node)), m_index(std::move(index)) { m_type = Type::Indexer; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("IndexerNode({}, {})", m_node->toDebugString(), m_index->toDebugString());
        }

        [[nodiscard]] std::unique_ptr<Node> const& node() const noexcept {
            return m_node;
        }

        [[nodiscard]] std::unique_ptr<Node> const& index() const noexcept {
            return m_index;
        }

    private:
        std::unique_ptr<Node> m_node;
        std::unique_ptr<Node> m_index;
    };

}

#endif // RIFT_INDEXER_NODE_HPP