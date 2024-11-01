#include <rift/value.hpp>
#include <cmath>
#include <sstream>
#include <fmt/format.h>

namespace rift {

    std::string Value::toString() const {
        switch (m_type) {
            case Type::String:
                return getString();
            case Type::Integer:
                return std::to_string(getInteger());
            case Type::Float:
                return fmt::format("{:.2f}", getFloat());
            case Type::Boolean:
                return getBoolean() ? "true" : "false";
            default:
                return "null";
        }
    }

    int64_t Value::toInteger() const {
        switch (m_type) {
            case Type::String:
                return util::readNumber<int64_t>(getString()).unwrapOr(0);
            case Type::Integer:
                return getInteger();
            case Type::Float:
                return static_cast<int64_t>(getFloat());
            case Type::Boolean:
                return getBoolean() ? 1 : 0;
            default:
                return 0;
        }
    }

    double Value::toFloat() const {
        switch (m_type) {
            case Type::String:
                return util::readNumber<double>(getString()).unwrapOr(0.0);
            case Type::Integer:
                return static_cast<double>(getInteger());
            case Type::Float:
                return getFloat();
            case Type::Boolean:
                return getBoolean() ? 1.0 : 0.0;
            default:
                return 0.0;
        }
    }

    bool Value::toBool() const {
        switch (m_type) {
            case Type::String:
                return !getString().empty();
            case Type::Integer:
                return getInteger() != 0;
            case Type::Float:
                return getFloat() != 0.0;
            case Type::Boolean:
                return getBoolean();
            default:
                return false;
        }
    }

    Value Value::operator+(const Value &other) const {
        // If either value is null, return null.
        if (isNull() || other.isNull()) {
            return null();
        }

        // If either value is a string, concatenate them.
        if (isString() || other.isString()) {
            return string(toString() + other.toString());
        }

        // If either value is a float, convert them to floats and add them.
        if (isFloat() || other.isFloat()) {
            return floating(toFloat() + other.toFloat());
        }

        // In other cases, add the integer values.
        return integer(toInteger() + other.toInteger());
    }

    Value Value::operator-(const Value &other) const {
        // If either value is null, return null.
        if (isNull() || other.isNull()) {
            return null();
        }

        // If either value is a string, we can't subtract them.
        if (isString() || other.isString()) {
            return string("<error: subtraction of strings>");
        }

        // If either value is a float, convert them to floats and subtract them.
        if (isFloat() || other.isFloat()) {
            return floating(toFloat() - other.toFloat());
        }

        // In other cases, subtract the integer values.
        return integer(toInteger() - other.toInteger());
    }

    Value Value::operator*(const Value &other) const {
        // If either value is null, return null.
        if (isNull() || other.isNull()) {
            return null();
        }

        // If either value is a string, repeat it n times, where n is the second value.
        if (isString() || other.isString()) {
            std::string result;
            auto count = other.isInteger() ? other.toInteger() : static_cast<int>(other.toFloat());
            for (int i = 0; i < count; ++i) {
                result += isString() ? getString() : other.getString();
            }
            return string(std::move(result));
        }

        // If either value is a float, convert them to floats and multiply them.
        if (isFloat() || other.isFloat()) {
            return floating(toFloat() * other.toFloat());
        }

        // In other cases, multiply the integer values.
        return integer(toInteger() * other.toInteger());
    }

    Value Value::operator/(const Value &other) const {
        // If either value is null, return null.
        if (isNull() || other.isNull()) {
            return null();
        }

        // If either value is a string, we can't divide them.
        if (isString() || other.isString()) {
            return string("<error: division of strings>");
        }

        // If either value is a float, convert them to floats and divide them.
        if (isFloat() || other.isFloat()) {
            // check for division by zero
            if (other.toFloat() == 0) {
                return floating(std::numeric_limits<double>::infinity());
            }
            return floating(toFloat() / other.toFloat());
        }

        // In other cases, divide the integer values.
        // check for division by zero
        if (other.toInteger() == 0) {
            return floating(std::numeric_limits<double>::infinity());
        }
        return integer(toInteger() / other.toInteger());
    }

    Value Value::operator%(const Value &other) const {
        // If either value is null, return null.
        if (isNull() || other.isNull()) {
            return null();
        }

        // If either value is a string, we can't divide them.
        if (isString() || other.isString()) {
            return string("<error: modulo of strings>");
        }

        // If either value is a float, convert them to floats and divide them.
        if (isFloat() || other.isFloat()) {
            // check for division by zero
            if (other.toFloat() == 0) {
                return floating(std::numeric_limits<double>::infinity());
            }
            return floating(std::fmod(toFloat(), other.toFloat()));
        }

        // In other cases, divide the integer values.
        // check for division by zero
        if (other.toInteger() == 0) {
            return floating(std::numeric_limits<double>::infinity());
        }
        return integer(toInteger() % other.toInteger());
    }

    Value Value::operator^(const Value &other) const {
        // If either value is null, return null.
        if (isNull() || other.isNull()) {
            return null();
        }

        // If either value is a string, we can't raise them to a power.
        if (isString() || other.isString()) {
            return string("<error: exponentiation of strings>");
        }

        // If either value is a float, convert them to floats and raise them to a power.
        if (isFloat() || other.isFloat()) {
            return floating(std::pow(toFloat(), other.toFloat()));
        }

        // In other cases, raise the integer values to a power.
        return integer(std::pow(toInteger(), other.toInteger()));
    }

    Value Value::operator==(const Value &other) const {
        // If both values are null, return true.
        if (isNull() && other.isNull()) {
            return boolean(true);
        }

        // If either value is null, return false.
        if (isNull() || other.isNull()) {
            return boolean(false);
        }

        // If either value is a string, compare them.
        if (isString() || other.isString()) {
            return boolean(toString() == other.toString());
        }

        // If either value is float, convert them to floats and compare them.
        if (isFloat() || other.isFloat()) {
            return boolean(toFloat() == other.toFloat());
        }

        // In other cases, compare the integer values.
        return boolean(toInteger() == other.toInteger());
    }

    Value Value::operator!=(const Value &other) const {
        // If both values are null, return false.
        if (isNull() && other.isNull()) {
            return boolean(false);
        }

        // If either value is null, return true.
        if (isNull() || other.isNull()) {
            return boolean(true);
        }

        // If either value is a string, compare them.
        if (isString() || other.isString()) {
            return boolean(toString() != other.toString());
        }

        // If either value is float, convert them to floats and compare them.
        if (isFloat() || other.isFloat()) {
            return boolean(toFloat() != other.toFloat());
        }

        // In other cases, compare the integer values.
        return boolean(toInteger() != other.toInteger());
    }

    Value Value::operator<(const Value &other) const {
        // If either value is null, return false.
        if (isNull() || other.isNull()) {
            return boolean(false);
        }

        // If either value is a string, compare them.
        if (isString() || other.isString()) {
            return boolean(toString() < other.toString());
        }

        // If either value is float, convert them to floats and compare them.
        if (isFloat() || other.isFloat()) {
            return boolean(toFloat() < other.toFloat());
        }

        // In other cases, compare the integer values.
        return boolean(toInteger() < other.toInteger());
    }

    Value Value::operator>(const Value &other) const {
        // If either value is null, return false.
        if (isNull() || other.isNull()) {
            return boolean(false);
        }

        // If either value is a string, compare them.
        if (isString() || other.isString()) {
            return boolean(toString() > other.toString());
        }

        // If either value is float, convert them to floats and compare them.
        if (isFloat() || other.isFloat()) {
            return boolean(toFloat() > other.toFloat());
        }

        // In other cases, compare the integer values.
        return boolean(toInteger() > other.toInteger());
    }

    Value Value::operator<=(const Value &other) const {
        // If either value is null, return false.
        if (isNull() || other.isNull()) {
            return boolean(false);
        }

        // If either value is a string, compare them.
        if (isString() || other.isString()) {
            return boolean(toString() <= other.toString());
        }

        // If either value is float, convert them to floats and compare them.
        if (isFloat() || other.isFloat()) {
            return boolean(toFloat() <= other.toFloat());
        }

        // In other cases, compare the integer values.
        return boolean(toInteger() <= other.toInteger());
    }

    Value Value::operator>=(const Value &other) const {
        // If either value is null, return false.
        if (isNull() || other.isNull()) {
            return boolean(false);
        }

        // If either value is a string, compare them.
        if (isString() || other.isString()) {
            return boolean(toString() >= other.toString());
        }

        // If either value is float, convert them to floats and compare them.
        if (isFloat() || other.isFloat()) {
            return boolean(toFloat() >= other.toFloat());
        }

        // In other cases, compare the integer values.
        return boolean(toInteger() >= other.toInteger());
    }

    Value Value::operator-() const {
        // If the value is null, return null.
        if (isNull()) {
            return null();
        }

        // If the value is a string, try to convert it to a float and negate it.
        if (isString()) {
            return floating(-toFloat());
        }

        // Negate the integer value.
        if (isInteger() || isBoolean()) {
            return integer(-toInteger());
        }

        return floating(-getFloat());
    }

    Value Value::operator&&(const Value &other) const {
        return boolean(toBool() && other.toBool());
    }

    Value Value::operator||(const Value &other) const {
        return boolean(toBool() || other.toBool());
    }

    Value Value::operator!() const {
        // If the value is null, return true.
        if (isNull()) {
            return boolean(true);
        }

        // If the value is a string, we can't perform logical NOT on it.
        if (isString()) {
            return boolean(false);
        }

        // In other cases, return the logical NOT of the boolean value.
        return boolean(!toBool());
    }
}
