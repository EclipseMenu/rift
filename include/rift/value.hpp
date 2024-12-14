#pragma once
#ifndef RIFT_VALUE_HPP
#define RIFT_VALUE_HPP

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#include <Geode/Result.hpp>

namespace rift {

    class Value;

    using Array = std::vector<Value>;
    using Object = std::unordered_map<std::string, Value>;

    /// @brief A value in the AST.
    class [[nodiscard]] Value {
    public:
        enum class Type {
            Null,
            String,
            Integer,
            Float,
            Boolean,
            Array,
            Object
        };

        constexpr Value() noexcept = default;
        constexpr Value(Value const&) noexcept = default;
        constexpr Value(Value&&) noexcept = default;

        explicit(false) constexpr Value(std::string value) noexcept
            : m_type(Type::String), m_data(std::move(value)) {}
        explicit(false) constexpr Value(std::string_view value) noexcept
            : m_type(Type::String), m_data(std::string(value)) {}
        explicit(false) constexpr Value(char const* value) noexcept
            : m_type(Type::String), m_data(std::string(value)) {}

        explicit(false) constexpr Value(int64_t value) noexcept
            : m_type(Type::Integer), m_data(value) {}
        explicit(false) constexpr Value(int value) noexcept
            : m_type(Type::Integer), m_data(value) {}

        explicit(false) constexpr Value(double value) noexcept
            : m_type(Type::Float), m_data(value) {}
        explicit(false) constexpr Value(float value) noexcept
            : m_type(Type::Float), m_data(value) {}

        explicit(false) constexpr Value(bool value) noexcept
            : m_type(Type::Boolean), m_data(value) {}

        explicit(false) constexpr Value(Array value) noexcept
            : m_type(Type::Array), m_data(std::move(value)) {}

        explicit(false) Value(Object value) noexcept
            : m_type(Type::Object), m_data(std::move(value)) {}

        template <typename T>
        static constexpr Value from(T&& value) noexcept {
            return Value(std::forward<T>(value));
        }

        static constexpr Value null() noexcept { return {}; }
        static constexpr Value string(std::string value) noexcept { return {std::move(value)}; }
        static constexpr Value integer(int64_t value) noexcept { return {value}; }
        static constexpr Value floating(double value) noexcept { return {value}; }
        static constexpr Value boolean(bool value) noexcept { return {value}; }
        static constexpr Value array(Array value) noexcept { return {std::move(value)}; }
        static Value object(Object value) noexcept { return {std::move(value)}; }

        /// @brief Returns true if the value is null.
        constexpr bool isNull() const noexcept { return m_type == Type::Null; }

        /// @brief Returns true if the value is a string.
        constexpr bool isString() const noexcept { return m_type == Type::String; }

        /// @brief Returns true if the value is a whole number.
        constexpr bool isInteger() const noexcept { return m_type == Type::Integer; }

        /// @brief Returns true if the value is a decimal number.
        constexpr bool isFloat() const noexcept { return m_type == Type::Float; }

        /// @brief Returns true if the value is boolean.
        constexpr bool isBoolean() const noexcept { return m_type == Type::Boolean; }

        /// @brief Returns true if the value is an array.
        constexpr bool isArray() const noexcept { return m_type == Type::Array; }

        /// @brief Returns true if the value is an object.
        constexpr bool isObject() const noexcept { return m_type == Type::Object; }

        /// @brief Returns the type of the value.
        constexpr Type type() const noexcept { return m_type; }

        /// @brief Reads the value as a string.
        /// @throws std::bad_variant_access if the value is not a string.
        constexpr const std::string& getString() const { return std::get<std::string>(m_data); }

        /// @brief Reads the value as an integer.
        /// @throws std::bad_variant_access if the value is not an integer.
        constexpr int64_t getInteger() const { return std::get<int64_t>(m_data); }

        /// @brief Reads the value as a float.
        /// @throws std::bad_variant_access if the value is not a float.
        constexpr double getFloat() const { return std::get<double>(m_data); }

        /// @brief Reads the value as a boolean.
        /// @throws std::bad_variant_access if the value is not a boolean.
        constexpr bool getBoolean() const { return std::get<bool>(m_data); }

        /// @brief Reads the value as an array.
        /// @throws std::bad_variant_access if the value is not an array.
        constexpr const Array& getArray() const { return std::get<Array>(m_data); }

        /// @brief Reads the value as an object.
        /// @throws std::bad_variant_access if the value is not an object.
        constexpr const Object& getObject() const { return std::get<Object>(m_data); }

        /// @brief Cast the value to a string.
        std::string toString() const noexcept;

        /// @brief Cast the value to an integer.
        int64_t toInteger() const noexcept;

        /// @brief Cast the value to a float.
        double toFloat() const noexcept;

        /// @brief Cast the value to a boolean.
        bool toBoolean() const noexcept;

        template <typename T>
        T to() const noexcept {
            using type = T;
            if constexpr (std::is_same_v<type, std::string>) {
                return toString();
            } else if constexpr (std::is_same_v<type, int64_t>) {
                return toInteger();
            } else if constexpr (std::is_same_v<type, double>) {
                return toFloat();
            } else if constexpr (std::is_same_v<type, bool>) {
                return toBoolean();
            } else if constexpr (std::is_same_v<type, Array>) {
                if (isArray()) return getArray();
                return Array{};
            } else if constexpr (std::is_same_v<type, Object>) {
                if (isObject()) return getObject();
                return Object{};
            } else {
                static_assert(false, "Unsupported type");
            }
            return {};
        }

        template <typename T>
        bool is() const noexcept {
            using type = std::remove_cvref_t<T>;
            if constexpr (std::is_same_v<type, std::string>) {
                return true;
            } else if constexpr (std::is_same_v<type, int64_t>) {
                return true;
            } else if constexpr (std::is_same_v<type, double>) {
                return true;
            } else if constexpr (std::is_same_v<type, bool>) {
                return true;
            } else if constexpr (std::is_same_v<type, Array>) {
                return isArray();
            } else if constexpr (std::is_same_v<type, Object>) {
                return isObject();
            } else {
                static_assert(false, "Unsupported type");
            }
            return false;
        }

        using Result = geode::Result<Value>;

        // Math operators

        Result operator+(const Value& other) const noexcept;
        Result operator-(const Value& other) const noexcept;
        Result operator*(const Value& other) const noexcept;
        Result operator/(const Value& other) const noexcept;
        Result operator%(const Value& other) const noexcept;
        Result operator^(const Value& other) const noexcept;

        // Comparison operators

        Value operator==(const Value& other) const noexcept;
        Value operator!=(const Value& other) const noexcept;
        Value operator<(const Value& other) const noexcept;
        Value operator>(const Value& other) const noexcept;
        Value operator<=(const Value& other) const noexcept;
        Value operator>=(const Value& other) const noexcept;

        // Logical operators

        Value operator&&(const Value& other) const noexcept;
        Value operator||(const Value& other) const noexcept;
        Value operator!() const noexcept;

        // Special operators

        Value operator-() const noexcept;
        Value at(const Value& key) const noexcept;
        Value operator->*(const Value& key) const noexcept;

        // Object/Array access operators

        Value operator[](size_t index) const noexcept;
        Value& operator[](size_t index) noexcept;
        Value operator[](std::string const& key) const noexcept;
        Value& operator[](std::string const& key) noexcept;

        // Cast operators

        operator bool() const noexcept { return toBoolean(); }
        explicit operator int64_t() const noexcept { return toInteger(); }
        explicit operator double() const noexcept { return toFloat(); }
        explicit operator std::string() const noexcept { return toString(); }

        constexpr Value& operator=(Value const&) noexcept = default;

    private:
        Type m_type = Type::Null;
        std::variant<std::monostate, std::string, int64_t, double, bool, Array, Object> m_data;
    };
}

#endif // RIFT_VALUE_HPP