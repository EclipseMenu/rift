#pragma once

#include <string>
#include <utility>

namespace rift {

    /// @brief A value in the AST.
    class Value {
    public:
        enum class Type {
            String,
            Integer,
            Float,
            Boolean,
        };

        static Value string(std::string value) {
            Value result;
            result.m_type = Type::String;
            result.m_string = std::move(value);
            return result;
        }

        static Value integer(int value) {
            Value result;
            result.m_type = Type::Integer;
            result.m_integer = value;
            return result;
        }

        static Value floating(float value) {
            Value result;
            result.m_type = Type::Float;
            result.m_float = value;
            return result;
        }

        static Value boolean(bool value) {
            Value result;
            result.m_type = Type::Boolean;
            result.m_boolean = value;
            return result;
        }

        template<typename T>
        static Value from(const T& value) {
            if constexpr (std::is_same_v<T, const std::string&> ||
                    std::is_same_v<T, std::string_view> ||
                    std::is_same_v<T, std::string> ||
                    std::is_same_v<T, char*> ||
                    std::is_same_v<T, const char*> ||
                    std::is_same_v<T, char[]>) {
                return string(value);
            } else if constexpr (std::is_same_v<T, int>) {
                return integer(value);
            } else if constexpr (std::is_same_v<T, float>) {
                return floating(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                return boolean(value);
            } else {
                static_assert(false, "Unsupported type");
            }
        }

        Value() = default;

        /// @brief Get the type of the value.
        /// @return The type of the value.
        [[nodiscard]] Type getType() const { return m_type; }

        /// @brief Get the string value.
        /// @return The string value.
        [[nodiscard]] const std::string& getString() const { return m_string; }

        /// @brief Get the integer value.
        /// @return The integer value.
        [[nodiscard]] int getInteger() const { return m_integer; }

        /// @brief Get the float value.
        /// @return The float value.
        [[nodiscard]] float getFloat() const { return m_float; }

        /// @brief Get the boolean value.
        /// @return The boolean value.
        [[nodiscard]] bool getBoolean() const { return m_boolean; }

        /// @brief Convert the value to a string.
        /// @return The string representation of the value.
        [[nodiscard]] std::string toString() const;

        /// @brief Convert the value to a float.
        /// @return The float representation of the value.
        [[nodiscard]] float toFloat() const;

        /// @brief Adds two values together.
        /// @param other The other value to add.
        /// @return The result of the addition.
        [[nodiscard]] Value operator+(const Value& other) const;

        /// @brief Subtracts two values.
        /// @param other The other value to subtract.
        /// @return The result of the subtraction.
        [[nodiscard]] Value operator-(const Value& other) const;

        /// @brief Multiplies two values.
        /// @param other The other value to multiply.
        /// @return The result of the multiplication.
        [[nodiscard]] Value operator*(const Value& other) const;

        /// @brief Divides two values.
        /// @param other The other value to divide.
        /// @return The result of the division.
        [[nodiscard]] Value operator/(const Value& other) const;

        /// @brief Modulus two values.
        /// @param other The other value to modulus.
        /// @return The result of the modulus.
        [[nodiscard]] Value operator%(const Value& other) const;

        /// @brief Exponentiate two values.
        /// @param other The other value to exponentiate.
        /// @return The result of the exponentiation.
        [[nodiscard]] Value operator^(const Value& other) const;

        /// @brief Compare two values for equality.
        /// @param other The other value to compare.
        /// @return The result of the comparison.
        [[nodiscard]] Value operator==(const Value& other) const;

        /// @brief Compare two values for inequality.
        /// @param other The other value to compare.
        /// @return The result of the comparison.
        [[nodiscard]] Value operator!=(const Value& other) const;

        /// @brief Compare two values for less than.
        /// @param other The other value to compare.
        /// @return The result of the comparison.
        [[nodiscard]] Value operator<(const Value& other) const;

        /// @brief Compare two values for greater than.
        /// @param other The other value to compare.
        /// @return The result of the comparison.
        [[nodiscard]] Value operator>(const Value& other) const;

        /// @brief Compare two values for less than or equal to.
        /// @param other The other value to compare.
        /// @return The result of the comparison.
        [[nodiscard]] Value operator<=(const Value& other) const;

        /// @brief Compare two values for greater than or equal to.
        /// @param other The other value to compare.
        /// @return The result of the comparison.
        [[nodiscard]] Value operator>=(const Value& other) const;

        /// @brief Negate a value.
        /// @return The negated value.
        [[nodiscard]] Value operator-() const;

        /// @brief Logical AND two values.
        /// @param other The other value to AND.
        /// @return The result of the AND operation.
        [[nodiscard]] Value operator&&(const Value& other) const;

        /// @brief Logical OR two values.
        /// @param other The other value to OR.
        /// @return The result of the OR operation.
        [[nodiscard]] Value operator||(const Value& other) const;

        /// @brief Logical NOT a value.
        /// @return The result of the NOT operation.
        [[nodiscard]] Value operator!() const;

    private:
        Type m_type = Type::String;
        std::string m_string; // strings have to be stored separately
        union {
            int m_integer{};
            float m_float;
            bool m_boolean;
        };
    };

}