#pragma once

#include "node.hpp"
#include "../value.hpp"

#include <string>
#include <utility>

namespace rift {

    /// @brief A node in the AST representing a constant value.
    class ValueNode : public Node {
    public:
        /// @brief Construct the value node.
        /// @param value The value of the node.
        explicit ValueNode(Value value) : m_value(std::move(value)) {}

        /// @brief Get the value of the node.
        /// @return The value of the node.
        [[nodiscard]] const Value& getValue() const { return m_value; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::Value; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override { return m_value; }

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            auto type = m_value.getType();
            out << "ValueNode(";
            switch (type) {
                case Value::Type::String:
                    out << "String(" << m_value.getString() << ')';
                    break;
                case Value::Type::Integer:
                    out << "Integer(" << m_value.getInteger() << ')';
                    break;
                case Value::Type::Float:
                    out << "Float(" << m_value.getFloat() << ')';
                    break;
                case Value::Type::Boolean:
                    out << "Boolean(" << m_value.getBoolean() << ')';
                    break;
            }
            out << ')';
            return out;
        }

    private:
        Value m_value;
    };

}