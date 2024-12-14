#pragma once
#ifndef RIFT_TERNARY_NODE_HPP
#define RIFT_TERNARY_NODE_HPP

#include "node.hpp"

namespace rift {

    class TernaryNode final : public Node {
    public:
        explicit TernaryNode(std::unique_ptr<Node> cond, std::unique_ptr<Node> trueBranch, std::unique_ptr<Node> falseBranch, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_cond(std::move(cond)), m_trueBranch(std::move(trueBranch)), m_falseBranch(std::move(falseBranch)) { m_type = Type::Ternary; }

        // constructor for null coalescing operator (??)
        // only has a true branch
        explicit TernaryNode(std::unique_ptr<Node> cond, std::unique_ptr<Node> trueBranch, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_cond(std::move(cond)), m_trueBranch(std::move(trueBranch)) { m_type = Type::Ternary; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format(
                "TernaryNode({}, {}, {})",
                m_cond->toDebugString(),
                m_trueBranch->toDebugString(),
                m_falseBranch ? m_falseBranch->toDebugString() : "null"
            );
        }

        [[nodiscard]] Node const& cond() const noexcept {
            return *m_cond;
        }

        [[nodiscard]] Node const& trueBranch() const noexcept {
            return *m_trueBranch;
        }

        [[nodiscard]] bool hasFalseBranch() const noexcept {
            return m_falseBranch != nullptr;
        }

        [[nodiscard]] Node const& falseBranch() const noexcept {
            return *m_falseBranch;
        }

    private:
        std::unique_ptr<Node> m_cond;
        std::unique_ptr<Node> m_trueBranch;
        std::unique_ptr<Node> m_falseBranch;
    };

}

#endif // RIFT_TERNARY_NODE_HPP