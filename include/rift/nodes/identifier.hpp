#pragma once

#include "node.hpp"

#include <string>
#include <utility>

namespace rift {

    /// @brief A node in the AST representing a variable identifier.
    class IdentifierNode : public Node {
    public:
        /// @brief Construct the identifier node.
        /// @param name The name of the identifier.
        explicit IdentifierNode(std::string name) : m_name(std::move(name)) {}

        /// @brief Get the name of the identifier.
        /// @return The name of the identifier.
        [[nodiscard]] const std::string& getName() const { return m_name; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::Identifier; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            return out << "IdentifierNode(" << m_name << ')';
        }

    private:
        std::string m_name;
    };

}