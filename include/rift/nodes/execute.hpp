#pragma once

#include "node.hpp"
#include "../value.hpp"

#include <string>
#include <utility>

namespace rift {

    /// @brief A node in the AST that should be evaluated as a sub-expression. (used for sub-string interpolation)
    class ExecuteNode : public Node {
    public:
        /// @brief Construct the execute node.
        /// @param child The child node to execute.
        explicit ExecuteNode(Node* child) : m_child(child) {}

        ~ExecuteNode() override {
            delete m_child;
        }

        /// @brief Get the child node to execute.
        /// @return The child node to execute.
        [[nodiscard]] const Node* getChild() const { return m_child; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::Execute; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            out << "ExecuteNode(" << *m_child << ')';
            return out;
        }

    private:
        Node* m_child;
    };

}