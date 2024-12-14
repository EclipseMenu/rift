#pragma once
#ifndef RIFT_CALL_NODE_HPP
#define RIFT_CALL_NODE_HPP

#include "node.hpp"

namespace rift {

    class CallNode final : public Node {
    public:
        explicit CallNode(std::unique_ptr<Node> node, std::vector<std::unique_ptr<Node>> args, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_node(std::move(node)), m_args(std::move(args)) { m_type = Type::Call; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            std::string result = fmt::format("CallNode({}", m_node->toDebugString());
            for (size_t i = 0; i < m_args.size(); ++i) {
                result += ", ";
                result += m_args[i]->toDebugString();
            }
            return result + ")";
        }

        [[nodiscard]] std::unique_ptr<Node> const& node() const noexcept {
            return m_node;
        }

        [[nodiscard]] std::vector<std::unique_ptr<Node>> const& args() const noexcept {
            return m_args;
        }

        [[nodiscard]] size_t numArgs() const noexcept {
            return m_args.size();
        }

    private:
        std::unique_ptr<Node> m_node;
        std::vector<std::unique_ptr<Node>> m_args;
    };

}

#endif // RIFT_CALL_NODE_HPP