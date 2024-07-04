#pragma once

#include "node.hpp"
#include "../token.hpp"

#include <string>
#include <utility>
#include <memory>

namespace rift {

    /// @brief A node in the AST representing a unary operation.
    class UnaryOpNode : public Node {
    public:
        /// @brief Construct the unary operation node.
        /// @param op The operator of the operation.
        /// @param operand The operand of the operation.
        UnaryOpNode(TokenType op, Node* operand)
            : m_op(op), m_operand(operand) {}

        ~UnaryOpNode() override {
            delete m_operand;
        }

        /// @brief Get the operator of the operation.
        /// @return The operator of the operation.
        [[nodiscard]] TokenType getOperator() const { return m_op; }

        /// @brief Get the operand of the operation.
        /// @return The operand of the operation.
        [[nodiscard]] const Node* getOperand() const { return m_operand; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::UnaryOp; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            return out << "UnaryOpNode(" << tokenTypeToString(m_op) << ", " << *m_operand << ')';
        }

    private:
        TokenType m_op;
        Node* m_operand;
    };

}