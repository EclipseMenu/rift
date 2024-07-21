#include <rift/value.hpp>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace rift {

    std::string Value::toString() const {
        switch (m_type) {
            case Type::String:
                return m_string;
            case Type::Integer:
                return std::to_string(m_integer);
            case Type::Float: {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << m_float;
                return ss.str();
            }
            case Type::Boolean:
                return m_boolean ? "true" : "false";
        }
    }

    float Value::toFloat() const {
        switch (m_type) {
            case Type::String:
                return std::stof(m_string);
            case Type::Integer:
                return static_cast<float>(m_integer);
            case Type::Float:
                return m_float;
            case Type::Boolean:
                return m_boolean ? 1.0f : 0.0f;
        }
    }

    Value Value::operator+(const Value& other) const {
        // If either value is a string, concatenate them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::string(toString() + other.toString());
        }

        // If either value is a boolean, convert them to integers and add them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::integer(m_integer + other.m_integer);
        }

        // If both values are numbers, add them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::integer(m_integer + other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::floating(m_float + other.m_float);
        }

        // If one of the values is a float, convert the other to a float and add them.
        if (m_type == Type::Float) {
            return Value::floating(m_float + other.toFloat());
        }

        return Value::floating(toFloat() + other.toFloat());
    }

    Value Value::operator-(const Value& other) const {
        // If either value is a string, we can't subtract them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::string("<error: subtraction of strings>");
        }

        // If either value is a boolean, convert them to integers and subtract them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::integer(m_integer - other.m_integer);
        }

        // If both values are numbers, subtract them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::integer(m_integer - other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::floating(m_float - other.m_float);
        }

        // If one of the values is a float, convert the other to a float and subtract them.
        if (m_type == Type::Float) {
            return Value::floating(m_float - other.toFloat());
        }

        return Value::floating(toFloat() - other.toFloat());
    }

    Value Value::operator*(const Value& other) const {
        // If first value is a string, repeat it n times where n is the second value.
        if (m_type == Type::String && (other.m_type == Type::Integer || other.m_type == Type::Float)) {
            std::string result;
            for (int i = 0; i < other.m_integer; ++i) {
                result += m_string;
            }
            return Value::string(result);
        } else if (other.m_type == Type::String && (m_type == Type::Integer || m_type == Type::Float)) {
            std::string result;
            for (int i = 0; i < m_integer; ++i) {
                result += other.m_string;
            }
            return Value::string(result);
        } else if (m_type == Type::String || other.m_type == Type::String) {
            return Value::string("<error: multiplication of strings>");
        }

        // If either value is a boolean, convert them to integers and multiply them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::integer(m_integer * other.m_integer);
        }

        // If both values are numbers, multiply them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::integer(m_integer * other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::floating(m_float * other.m_float);
        }

        // If one of the values is a float, convert the other to a float and multiply them.
        if (m_type == Type::Float) {
            return Value::floating(m_float * other.toFloat());
        }

        return Value::floating(toFloat() * other.toFloat());
    }

    Value Value::operator/(const Value& other) const {
        // If either value is a string, we can't divide them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::string("<error: division of strings>");
        }

        // If either value is a boolean, convert them to integers and divide them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::integer(m_integer / other.m_integer);
        }

        // If both values are numbers, divide them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::integer(m_integer / other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::floating(m_float / other.m_float);
        }

        // If one of the values is a float, convert the other to a float and divide them.
        if (m_type == Type::Float) {
            return Value::floating(m_float / other.toFloat());
        }

        return Value::floating(toFloat() / other.toFloat());
    }

    Value Value::operator%(const Value& other) const {
        // If either value is a string, we can't divide them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::string("<error: modulo of strings>");
        }

        // If either value is a boolean, convert them to integers and divide them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::integer(m_integer % other.m_integer);
        }

        // If both values are numbers, divide them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::integer(m_integer % other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::floating(std::fmod(m_float, other.m_float));
        }

        // If one of the values is a float, convert the other to a float and divide them.
        if (m_type == Type::Float) {
            return Value::floating(std::fmod(m_float, other.toFloat()));
        }

        return Value::floating(std::fmod(toFloat(), other.toFloat()));
    }

    Value Value::operator^(const rift::Value &other) const {
        // If either value is a string, we can't raise them to a power.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::string("<error: exponentiation of strings>");
        }

        // If either value is a boolean, convert them to integers and raise them to a power.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::integer(std::pow(m_integer, other.m_integer));
        }

        // If both values are numbers, raise them to a power.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::integer(std::pow(m_integer, other.m_integer));
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::floating(std::pow(m_float, other.m_float));
        }

        // If one of the values is a float, convert the other to a float and raise them to a power.
        if (m_type == Type::Float) {
            return Value::floating(std::pow(m_float, other.toFloat()));
        }

        return Value::floating(std::pow(toFloat(), other.toFloat()));
    }

    Value Value::operator==(const Value& other) const {
        // If either value is a string, compare them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(toString() == other.toString());
        }

        // If either value is a boolean, convert them to integers and compare them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::boolean(m_integer == other.m_integer);
        }

        // If both values are numbers, compare them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::boolean(m_integer == other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::boolean(m_float == other.m_float);
        }

        // If one of the values is a float, convert the other to a float and compare them.
        if (m_type == Type::Float) {
            return Value::boolean(m_float == other.toFloat());
        }

        return Value::boolean(toFloat() == other.toFloat());
    }

    Value Value::operator!=(const Value& other) const {
        // If either value is a string, compare them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(toString() != other.toString());
        }

        // If either value is a boolean, convert them to integers and compare them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::boolean(m_integer != other.m_integer);
        }

        // If both values are numbers, compare them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::boolean(m_integer != other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::boolean(m_float != other.m_float);
        }

        // If one of the values is a float, convert the other to a float and compare them.
        if (m_type == Type::Float) {
            return Value::boolean(m_float != other.toFloat());
        }

        return Value::boolean(toFloat() != other.toFloat());
    }

    Value Value::operator<(const Value& other) const {
        // If either value is a string, compare them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(toString() < other.toString());
        }

        // If either value is a boolean, convert them to integers and compare them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::boolean(m_integer < other.m_integer);
        }

        // If both values are numbers, compare them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::boolean(m_integer < other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::boolean(m_float < other.m_float);
        }

        // If one of the values is a float, convert the other to a float and compare them.
        if (m_type == Type::Float) {
            return Value::boolean(m_float < other.toFloat());
        }

        return Value::boolean(toFloat() < other.toFloat());
    }

    Value Value::operator>(const Value& other) const {
        // If either value is a string, compare them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(toString() > other.toString());
        }

        // If either value is a boolean, convert them to integers and compare them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::boolean(m_integer > other.m_integer);
        }

        // If both values are numbers, compare them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::boolean(m_integer > other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::boolean(m_float > other.m_float);
        }

        // If one of the values is a float, convert the other to a float and compare them.
        if (m_type == Type::Float) {
            return Value::boolean(m_float > other.toFloat());
        }

        return Value::boolean(toFloat() > other.toFloat());
    }

    Value Value::operator<=(const Value& other) const {
        // If either value is a string, compare them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(toString() <= other.toString());
        }

        // If either value is a boolean, convert them to integers and compare them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::boolean(m_integer <= other.m_integer);
        }

        // If both values are numbers, compare them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::boolean(m_integer <= other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::boolean(m_float <= other.m_float);
        }

        // If one of the values is a float, convert the other to a float and compare them.
        if (m_type == Type::Float) {
            return Value::boolean(m_float <= other.toFloat());
        }

        return Value::boolean(toFloat() <= other.toFloat());
    }

    Value Value::operator>=(const Value& other) const {
        // If either value is a string, compare them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(toString() >= other.toString());
        }

        // If either value is a boolean, convert them to integers and compare them.
        if (m_type == Type::Boolean || other.m_type == Type::Boolean) {
            return Value::boolean(m_integer >= other.m_integer);
        }

        // If both values are numbers, compare them.
        if (m_type == Type::Integer && other.m_type == Type::Integer) {
            return Value::boolean(m_integer >= other.m_integer);
        }

        if (m_type == Type::Float && other.m_type == Type::Float) {
            return Value::boolean(m_float >= other.m_float);
        }

        // If one of the values is a float, convert the other to a float and compare them.
        if (m_type == Type::Float) {
            return Value::boolean(m_float >= other.toFloat());
        }

        return Value::boolean(toFloat() >= other.toFloat());
    }

    Value Value::operator&&(const Value& other) const {
        // If either value is a string, we can't perform logical AND on them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(false);
        }

        // If either value is a boolean, perform logical AND on them.
        if (m_type == Type::Boolean && other.m_type == Type::Boolean) {
            return Value::boolean(m_boolean && other.m_boolean);
        }

        // If both values are numbers, convert them to booleans and perform logical AND on them.
        return Value::boolean(m_integer && other.m_integer);
    }

    Value Value::operator||(const Value& other) const {
        // If either value is a string, we can't perform logical OR on them.
        if (m_type == Type::String || other.m_type == Type::String) {
            return Value::boolean(false);
        }

        // If either value is a boolean, perform logical OR on them.
        if (m_type == Type::Boolean && other.m_type == Type::Boolean) {
            return Value::boolean(m_boolean || other.m_boolean);
        }

        // If both values are numbers, convert them to booleans and perform logical OR on them.
        return Value::boolean(m_integer || other.m_integer);
    }

    Value Value::operator!() const {
        // If the value is a string, we can't perform logical NOT on it.
        if (m_type == Type::String) {
            return Value::boolean(false);
        }

        // If the value is a boolean, perform logical NOT on it.
        if (m_type == Type::Boolean) {
            return Value::boolean(!m_boolean);
        }

        // If the value is a number, convert it to a boolean and perform logical NOT on it.
        return Value::boolean(!m_integer);
    }

    Value Value::operator-() const {
        // If the value is a string, we can't negate it.
        if (m_type == Type::String) {
            return Value::floating(-toFloat());
        }

        // If the value is a boolean, convert it to an integer and negate it.
        if (m_type == Type::Boolean) {
            return Value::integer(-m_integer);
        }

        // If the value is a number, negate it.
        if (m_type == Type::Integer) {
            return Value::integer(-m_integer);
        }

        return Value::floating(-m_float);
    }

}