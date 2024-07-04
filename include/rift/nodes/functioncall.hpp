#pragma once

#include "node.hpp"
#include "../token.hpp"

#include <string>
#include <utility>
#include <memory>
#include <vector>

namespace rift {

    /// @brief A node in the AST representing a function call.
    class FunctionCallNode : public Node {
    public:
        /// @brief Construct the function call node.
        /// @param name The name of the function.
        /// @param arguments The arguments of the function.
        FunctionCallNode(Node* name, std::vector<Node*> arguments)
            : m_name(name), m_arguments(std::move(arguments)) {}

        ~FunctionCallNode() override {
            delete m_name;
            for (Node* argument : m_arguments) {
                delete argument;
            }
        }

        /// @brief Get the name of the function.
        /// @return The name of the function.
        [[nodiscard]] const Node* getName() const { return m_name; }

        /// @brief Get the arguments of the function.
        /// @return The arguments of the function.
        [[nodiscard]] const std::vector<Node*>& getArguments() const { return m_arguments; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::FunctionCall; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            out << "FunctionCallNode(" << *m_name << ", [";
            for (size_t i = 0; i < m_arguments.size(); ++i) {
                out << *m_arguments[i];
                if (i + 1 < m_arguments.size()) {
                    out << ", ";
                }
            }
            return out << "])";
        }

    private:
        Node* m_name;
        std::vector<Node*> m_arguments;
    };

}