#pragma once

#include "node.hpp"
#include "../value.hpp"

#include <vector>
#include <string>
#include <utility>

namespace rift {

    /// @brief A root node in the AST.
    class RootNode : public Node {
    public:
        /// @brief Construct the root node.
        RootNode() = default;

        /// @brief Destructor.
        ~RootNode() override {
            for (auto* child : m_children) {
                delete child;
            }
        }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::Root; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        /// @brief Add a child to the root node.
        /// @param child The child to add.
        void addChild(Node* child) { m_children.push_back(child); }

        /// @brief Get the children of the root node.
        /// @return The children of the root node.
        [[nodiscard]] const std::vector<Node*>& getChildren() const { return m_children; }

        std::ostream& print(std::ostream& out) const override {
            out << "RootNode(\n  ";
            for (const auto* child : m_children) {
                out << *child << ", " << std::endl;
                if (child != m_children.back()) {
                    out << "  ";
                }
            }
            out << ")";
            return out;
        }

    private:
        std::vector<Node*> m_children;
    };

}