#pragma once

#include "node.hpp"
#include "../token.hpp"

namespace rift {

    /// @brief A node in the AST representing a binary operation.
    class BinaryOpNode : public Node {
    public:
        /// @brief Construct the binary operation node.
        /// @param lhs The left-hand side of the operation.
        /// @param rhs The right-hand side of the operation.
        /// @param op The operator.
        BinaryOpNode(Node* lhs, Node* rhs, TokenType op)
            : m_lhs(lhs), m_rhs(rhs), m_op(op) {}

        ~BinaryOpNode() override {
            delete m_lhs;
            delete m_rhs;
        }

        /// @brief Get the left-hand side of the operation.
        /// @return The left-hand side of the operation.
        [[nodiscard]] const Node* getLhs() const { return m_lhs; }

        /// @brief Get the right-hand side of the operation.
        /// @return The right-hand side of the operation.
        [[nodiscard]] const Node* getRhs() const { return m_rhs; }

        /// @brief Get the operator.
        /// @return The operator.
        [[nodiscard]] TokenType getOp() const { return m_op; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::BinaryOp; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            out << "BinaryOpNode(" << *m_lhs << ", " << *m_rhs << ", " << tokenTypeToString(m_op) << ")";
            return out;
        }

    private:
        Node* m_lhs;
        Node* m_rhs;
        TokenType m_op;
    };

}