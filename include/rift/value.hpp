#pragma once

#include <charconv>
#include <string>
#include <utility>

#include "result.hpp"

namespace rift {

    namespace util {
        template <typename T>
        constexpr bool isStringType() {
            using type = std::remove_cvref_t<T>;
            return std::is_same_v<type, std::string> ||
                   std::is_same_v<type, std::string_view> ||
                   std::is_same_v<type, char*> ||
                   std::is_same_v<type, const char*>;
        }

        template <typename T>
        concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;

        /// @brief Read a number from a string.
        /// @note This code is taken from the
        /// <a href="https://github.com/geode-sdk/geode/blob/fd2a457e76a2d4ef6958ea83d0d5a006ac1e2dfc/loader/include/Geode/utils/general.hpp#L128">
        /// Geode SDK `numFromString` function.
        /// </a>
        template <Number Num>
        Result<Num> readNumber(std::string_view const str) {
            if constexpr (std::is_floating_point_v<Num>
                #if defined(__cpp_lib_to_chars)
                    && false
                #endif
            ) {
                Num val;
                char* strEnd;
                errno = 0;
                if constexpr (std::is_same_v<Num, float>) val = std::strtof(str.data(), &strEnd);
                else if constexpr (std::is_same_v<Num, double>) val = std::strtod(str.data(), &strEnd);
                else if constexpr (std::is_same_v<Num, long double>) val = std::strtold(str.data(), &strEnd);
                if (errno == ERANGE) return Err("Number is too large to fit");
                if (strEnd == str.data()) return Err("String is not a number");
                return val;
            } else {
                Num result;
                std::from_chars_result res;
                if constexpr (std::is_floating_point_v<Num>) res = std::from_chars(str.data(), str.data() + str.size(), result);
                else res = std::from_chars(str.data(), str.data() + str.size(), result);

                auto [ptr, ec] = res;
                if (ec == std::errc()) return result;
                if (ptr != str.data() + str.size()) return Err("String contains trailing extra data");
                if (ec == std::errc::invalid_argument) return Err("String is not a number");
                if (ec == std::errc::result_out_of_range) return Err("Number is too large to fit");
                return Err("Unknown error");
            }
        }
    }

    /// @brief A value in the AST.
    class [[nodiscard]] Value {
    public:
        enum class Type {
            Null,
            String,
            Integer,
            Float,
            Boolean,
        };

    private:
        Value(Type type, std::variant<int64_t, double, bool, std::string, std::monostate> value)
            : m_type(type), m_value(std::move(value)) {}

    public:
        static Value null() {
            return Value(Type::Null, std::monostate{});
        }

        static Value string(std::string value) {
            return Value(Type::String, std::move(value));
        }

        static Value integer(int64_t value) {
            return Value(Type::Integer, value);
        }

        static Value floating(double value) {
            return Value(Type::Float, value);
        }

        static Value boolean(bool value) {
            return Value(Type::Boolean, value);
        }

        template <typename T>
        static Value from(T value) {
            if constexpr (util::isStringType<T>()) {
                return string(std::move(value));
            } else if constexpr (std::is_integral_v<T>) {
                return integer(value);
            } else if constexpr (std::is_floating_point_v<T>) {
                return floating(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                return boolean(value);
            } else {
                static_assert(!sizeof(T*), "Unsupported type 'T' for Value::from");
            }
            return null();
        }

        template <size_t N>
        static Value from(const char(&value)[N]) {
            return string(value);
        }

        bool isString() const { return m_type == Type::String; }
        bool isInteger() const { return m_type == Type::Integer; }
        bool isFloat() const { return m_type == Type::Float; }
        bool isBoolean() const { return m_type == Type::Boolean; }
        bool isNull() const { return m_type == Type::Null; }

        Type getType() const { return m_type; }

        const std::string& getString() const { return std::get<std::string>(m_value); }
        int64_t getInteger() const { return std::get<int64_t>(m_value); }
        double getFloat() const { return std::get<double>(m_value); }
        bool getBoolean() const { return std::get<bool>(m_value); }

        std::string toString() const;
        int64_t toInteger() const;
        double toFloat() const;
        bool toBool() const;

        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
        Value operator%(const Value& other) const;
        Value operator^(const Value& other) const;

        Value operator==(const Value& other) const;
        Value operator!=(const Value& other) const;
        Value operator<(const Value& other) const;
        Value operator>(const Value& other) const;
        Value operator<=(const Value& other) const;
        Value operator>=(const Value& other) const;

        Value operator-() const;
        Value operator&&(const Value& other) const;
        Value operator||(const Value& other) const;
        Value operator!() const;

    private:
        Type m_type = Type::Null;
        std::variant<int64_t, double, bool, std::string, std::monostate> m_value;
    };

}
