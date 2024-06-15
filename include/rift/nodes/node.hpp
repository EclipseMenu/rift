#pragma once

namespace rift {

    /// @brief A node in the AST.
    class Node {
    public:
        /// @brief The type of the node.
        enum class Type {
            Segment,
            Identifier
        };

        /// @brief Destruct the node.
        virtual ~Node() = default;

        /// @brief Get the type of the node.
        /// @return The type of the node.
        [[nodiscard]] virtual Type getType() const = 0;

        /// @brief Accept a visitor.
        /// @param visitor The visitor.
        virtual void accept(class Visitor* visitor) = 0;
    };

}