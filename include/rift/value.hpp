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
            Boolean
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
                    std::is_same_v<T, const char*>) {
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