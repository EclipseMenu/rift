#pragma once
#ifndef RIFT_VALUE_NODE_HPP
#define RIFT_VALUE_NODE_HPP

#include "node.hpp"

namespace rift {

    class ValueNode final : public Node {
    public:
        explicit ValueNode(Value value, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_value(std::move(value)) { m_type = Type::Value; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("ValueNode({})", m_value.toString());
        }

        [[nodiscard]] Value const& value() const noexcept {
            return m_value;
        }

    private:
        Value m_value;
    };

}

#endif // RIFT_VALUE_NODE_HPP