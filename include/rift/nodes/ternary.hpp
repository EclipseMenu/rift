#pragma once

#include "node.hpp"

#include <string>
#include <utility>
#include <memory>

namespace rift {

    /// @brief A node in the AST representing a ternary operation.
    class TernaryNode : public Node {
    public:
        /// @brief Construct the ternary operation node.
        /// @param condition The condition of the operation.
        /// @param trueBranch The true branch of the operation.
        /// @param falseBranch The false branch of the operation.
        TernaryNode(Node* condition, Node* trueBranch, Node* falseBranch)
            : m_condition(condition), m_trueBranch(trueBranch), m_falseBranch(falseBranch) {}

        ~TernaryNode() override {
            delete m_condition;
            delete m_trueBranch;
            delete m_falseBranch;
        }

        /// @brief Get the condition of the operation.
        /// @return The condition of the operation.
        [[nodiscard]] const Node* getCondition() const { return m_condition; }

        /// @brief Get the true branch of the operation.
        /// @return The true branch of the operation.
        [[nodiscard]] const Node* getTrueBranch() const { return m_trueBranch; }

        /// @brief Get the false branch of the operation.
        /// @return The false branch of the operation.
        [[nodiscard]] const Node* getFalseBranch() const { return m_falseBranch; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::TernaryOp; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        std::ostream& print(std::ostream& out) const override {
            return out << "TernaryNode(" << *m_condition << ", " << *m_trueBranch << ", " << *m_falseBranch << ')';
        }

    private:
        Node* m_condition;
        Node* m_trueBranch;
        Node* m_falseBranch;
    };

}