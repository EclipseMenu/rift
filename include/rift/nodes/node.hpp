#pragma once

#include <iostream>
#include "../value.hpp"

namespace rift {

    /// @brief A node in the AST.
    class Node {
    public:
        /// @brief The type of the node.
        enum class Type {
            Root,
            Segment,
            Identifier,
            Value,
            UnaryOp,
            BinaryOp,
            FunctionCall,
            TernaryOp,
        };

        /// @brief Destruct the node.
        virtual ~Node() = default;

        /// @brief Get the type of the node.
        /// @return The type of the node.
        [[nodiscard]] virtual Type getType() const = 0;

        /// @brief Accept a visitor.
        /// @param visitor The visitor.
        virtual void accept(class Visitor* visitor) = 0;

        /// @brief Get the value of the node.
        /// @param visitor The visitor. Used if the node needs to get variables.
        /// @return The value of the node.
        [[nodiscard]] virtual Value getValue(Visitor* visitor) const = 0;

        /// @brief Print the node to an output stream. (For debugging)
        virtual std::ostream& print(std::ostream& out) const = 0;

        friend std::ostream& operator<<(std::ostream& out, const Node& node) {
            return node.print(out);
        }
    };

}