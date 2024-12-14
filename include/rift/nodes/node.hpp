#pragma once
#ifndef RIFT_NODE_HPP
#define RIFT_NODE_HPP

#include <string>
#include "../value.hpp"
#include "../errors/runtime.hpp"

#include <Geode/Result.hpp>

namespace rift {

    class Visitor;

    using RuntimeResult = geode::Result<Value, RuntimeError>;

    class Node {
    public:
        Node(size_t fromIndex, size_t toIndex) noexcept
            : m_fromIndex(fromIndex), m_toIndex(toIndex) {}
        Node(Node const&) = delete;
        Node(Node&&) = delete;

        enum class Type {
            Segment,       // Static text segment
            Root,          // Combines multiple nodes and returns the string
            Identifier,    // Variable identifier
            Binary,        // Binary operation
            Unary,         // Unary operation
            Ternary,       // Ternary operation
            Call,          // Function call
            Accessor,      // Accessor (for objects)
            Indexer,       // Indexer (for arrays)
            Value          // Literal value
        };

        virtual ~Node() = default;

        /// @brief Returns the type of the node.
        /// @return the type of the node
        [[nodiscard]] Type type() const noexcept { return m_type; }

        /// @brief Construct a RuntimeError with the given message.
        /// @param message the error message
        /// @return a RuntimeError with the given message
        [[nodiscard]] RuntimeResult error(std::string message) const noexcept {
            return geode::Err(RuntimeError(std::move(message), m_fromIndex, m_toIndex));
        }

        /// @brief Returns a string representation of the node for debugging.
        /// @return a string representation of the node
        [[nodiscard]] virtual std::string toDebugString() const noexcept = 0;

        std::ostream& operator<<(std::ostream& os) const noexcept {
            return os << toDebugString();
        }

        friend std::ostream& operator<<(std::ostream& os, Node const& node) noexcept {
            return node.operator<<(os);
        }

    protected:
        Type m_type = Type::Segment;
        size_t m_fromIndex, m_toIndex;
    };
}

template<>
struct fmt::formatter<rift::Node::Type> : formatter<std::string_view> {
    template<typename FormatContext>
    auto format(rift::Node::Type type, FormatContext& ctx) const {
        std::string_view name = "Unknown";
        switch (type) {
            case rift::Node::Type::Segment: name = "Segment"; break;
            case rift::Node::Type::Root: name = "Root"; break;
            case rift::Node::Type::Identifier: name = "Identifier"; break;
            case rift::Node::Type::Binary: name = "Binary"; break;
            case rift::Node::Type::Unary: name = "Unary"; break;
            case rift::Node::Type::Ternary: name = "Ternary"; break;
            case rift::Node::Type::Call: name = "Call"; break;
            case rift::Node::Type::Accessor: name = "Accessor"; break;
            case rift::Node::Type::Indexer: name = "Indexer"; break;
            case rift::Node::Type::Value: name = "Value"; break;
        }
        return formatter<std::string_view>::format(name, ctx);
    }
};

#endif // RIFT_NODE_HPP