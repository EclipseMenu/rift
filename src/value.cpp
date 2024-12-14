#include <rift/value.hpp>
#include <rift/util.hpp>

#include <fmt/format.h>
#include <algorithm>

namespace rift {

    std::string Value::toString() const noexcept {
        switch (m_type) {
            case Type::String:
                return std::get<std::string>(m_data);
            case Type::Integer:
                return std::to_string(std::get<int64_t>(m_data));
            case Type::Float:
                return fmt::format("{:.2f}", std::get<double>(m_data));
            case Type::Boolean:
                return std::get<bool>(m_data) ? "true" : "false";
            case Type::Array: {
                std::string result = "[";
                auto const& array = getArray();
                for (size_t i = 0; i < array.size(); ++i) {
                    result += array[i].toString();
                    if (i + 1 < array.size()) result += ", ";
                }
                return result + "]";
            }
            case Type::Object: {
                // print all key-value pairs in the object on separate lines
                std::string result = "{";
                for (auto const& [key, value] : getObject()) {
                    result += fmt::format("{}: {}, ", key, value.toString());
                }
                if (!result.empty()) result.resize(result.size() - 2);
                return result + "}";
            }
            default:
                return "null";
        }
    }

    int64_t Value::toInteger() const noexcept {
        switch (m_type) {
            case Type::String:
                return util::readNumber<int64_t>(std::get<std::string>(m_data)).unwrapOrDefault();
            case Type::Integer:
                return std::get<int64_t>(m_data);
            case Type::Float:
                return static_cast<int64_t>(std::get<double>(m_data));
            case Type::Boolean:
                return std::get<bool>(m_data) ? 1 : 0;
            case Type::Array:
                return static_cast<int64_t>(std::get<Array>(m_data).size());
            case Type::Object:
                return static_cast<int64_t>(std::get<Object>(m_data).size());
            default:
                return 0;
        }
    }

    double Value::toFloat() const noexcept {
        switch (m_type) {
            case Type::String:
                return util::readNumber<double>(std::get<std::string>(m_data)).unwrapOr(std::numeric_limits<double>::quiet_NaN());
            case Type::Integer:
                return static_cast<double>(std::get<int64_t>(m_data));
            case Type::Float:
                return std::get<double>(m_data);
            case Type::Boolean:
                return std::get<bool>(m_data) ? 1.0 : 0.0;
            case Type::Array:
                return static_cast<double>(std::get<Array>(m_data).size());
            case Type::Object:
                return static_cast<double>(std::get<Object>(m_data).size());
            default:
                return 0.0;
        }
    }

    bool Value::toBoolean() const noexcept {
        switch (m_type) {
            case Type::String:
                return !std::get<std::string>(m_data).empty();
            case Type::Integer:
                return std::get<int64_t>(m_data) != 0;
            case Type::Float:
                return std::get<double>(m_data) != 0.0;
            case Type::Boolean:
                return std::get<bool>(m_data);
            case Type::Array:
                return !std::get<Array>(m_data).empty();
            case Type::Object:
                return !std::get<Object>(m_data).empty();
            default:
                return false;
        }
    }

    Value::Result Value::operator+(const Value& other) const noexcept {
        // if either value is null, return null
        if (isNull() || other.isNull()) {
            return geode::Ok(Value());
        }

        // if either value is an object, return an error
        if (isObject() || other.isObject()) {
            return geode::Err("Cannot perform object addition");
        }

        // if either value is an array, concatenate them
        if (isArray() || other.isArray()) {
            // if both values are arrays, concatenate them
            if (isArray() && other.isArray()) {
                auto result = getArray();
                result.insert(result.end(), other.getArray().begin(), other.getArray().end());
                return geode::Ok(Value(result));
            }

            // if one value is an array, append the other value
            auto const& array = isArray() ? getArray() : other.getArray();
            auto result = array;
            result.push_back(isArray() ? other : *this);
            return geode::Ok(Value(result));
        }

        // if either value is a string, concatenate them
        if (isString() || other.isString()) {
            return geode::Ok(Value(toString() + other.toString()));
        }

        // if either value is a float, convert both to floats and add
        if (isFloat() || other.isFloat()) {
            return geode::Ok(Value(toFloat() + other.toFloat()));
        }

        // otherwise, convert both to integers and add
        return geode::Ok(Value(toInteger() + other.toInteger()));
    }

    Value::Result Value::operator-(const Value& other) const noexcept {
        // if either value is null, return null
        if (isNull() || other.isNull()) {
            return geode::Ok(Value());
        }

        // if either value is an object, return an error
        if (isObject() || other.isObject()) {
            return geode::Err("Cannot perform object subtraction");
        }

        // if either value is an array, remove all occurrences of the other value
        if (isArray() || other.isArray()) {
            // make sure only one of the values is an array
            if (isArray() && other.isArray()) {
                return geode::Err("Cannot subtract two arrays");
            }

            auto const& array = isArray() ? getArray() : other.getArray();
            auto const& value = isArray() ? other : *this;

            Array result;
            result.reserve(array.size());

            for (auto const& item : array) {
                if (item != value) {
                    result.push_back(item);
                }
            }

            return geode::Ok(Value(result));
        }

        // if either value is a string, remove all occurrences of the other value
        if (isString() || other.isString()) {
            // if both values are strings, remove all occurrences of the other string
            if (isString() && other.isString()) {
                auto result = getString();
                auto const& str = other.getString();

                // if the value is empty, return the string
                if (str.empty()) {
                    return geode::Ok(Value(result));
                }

                size_t pos = 0;
                while ((pos = result.find(str, pos)) != std::string::npos) {
                    result.erase(pos, str.size());
                }

                return geode::Ok(Value(result));
            }

            // if first value is not a string, return error
            if (!isString()) {
                return geode::Err("Cannot subtract a string from a non-string");
            }

            // subtract the other value from the string
            auto str = getString();
            auto const& value = other.toString();

            if (value.empty()) {
                return geode::Ok(Value(str));
            }

            size_t pos = 0;
            while ((pos = str.find(value, pos)) != std::string::npos) {
                str.erase(pos, value.size());
            }

            return geode::Ok(Value(str));
        }

        // if either value is a float, convert both to floats and subtract
        if (isFloat() || other.isFloat()) {
            return geode::Ok(Value(toFloat() - other.toFloat()));
        }

        // otherwise, convert both to integers and subtract
        return geode::Ok(Value(toInteger() - other.toInteger()));
    }

    Value::Result Value::operator*(const Value& other) const noexcept {
        // if either value is null, return null
        if (isNull() || other.isNull()) {
            return geode::Ok(Value());
        }

        // if either value is an object, return an error
        if (isObject() || other.isObject()) {
            return geode::Err("Cannot perform object multiplication");
        }

        // if either value is an array, repeat it n times, where n is the other value
        if (isArray() || other.isArray()) {
            // make sure only one of the values is an array
            if (isArray() && other.isArray()) {
                return geode::Err("Cannot multiply two arrays");
            }

            auto const& array = isArray() ? getArray() : other.getArray();
            auto const& num = isArray() ? other.toInteger() : toInteger();

            Array result;
            result.reserve(array.size() * num);

            for (int64_t i = 0; i < num; ++i) {
                result.insert(result.end(), array.begin(), array.end());
            }

            return geode::Ok(Value(result));
        }

        // if either value is a string, repeat it n times, where n is the other value
        if (isString() || other.isString()) {
            // make sure only one of the values is a string
            if (isString() && other.isString()) {
                return geode::Err("Cannot multiply two strings");
            }

            auto const& str = isString() ? getString() : other.getString();
            auto const& num = isString() ? other.toInteger() : toInteger();

            std::string result;
            result.reserve(str.size() * num);

            for (int64_t i = 0; i < num; ++i) {
                result += str;
            }

            return geode::Ok(Value(result));
        }

        // if either value is a float, convert both to floats and multiply
        if (isFloat() || other.isFloat()) {
            return geode::Ok(Value(toFloat() * other.toFloat()));
        }

        // otherwise, convert both to integers and multiply
        return geode::Ok(Value(toInteger() * other.toInteger()));
    }

    Value::Result Value::operator/(const Value& other) const noexcept {
        // if either value is null, return null
        if (isNull() || other.isNull()) {
            return geode::Ok(Value());
        }

        // if either value is an object, return an error
        if (isObject() || other.isObject()) {
            return geode::Err("Cannot perform object division");
        }

        // if either value is an array, return an error
        if (isArray() || other.isArray()) {
            return geode::Err("Cannot perform array division");
        }

        // if either value is a string, return an error
        if (isString() || other.isString()) {
            return geode::Err("Cannot perform string division");
        }

        // if either value is a float, convert both to floats and divide
        if (isFloat() || other.isFloat()) {
            // if the divisor is zero, return Infinity or -Infinity
            if (other.toFloat() == 0.0) {
                return geode::Ok(Value(toFloat() < 0.0 ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity()));
            }
            return geode::Ok(Value(toFloat() / other.toFloat()));
        }

        // otherwise, convert both to integers and divide
        // if the divisor is zero, return Infinity or -Infinity
        if (other.toInteger() == 0) {
            return geode::Ok(Value(toInteger() < 0 ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity()));
        }
        return geode::Ok(Value(toInteger() / other.toInteger()));
    }

    Value::Result Value::operator%(const Value& other) const noexcept {
        // if either value is null, return null
        if (isNull() || other.isNull()) {
            return geode::Ok(Value());
        }

        // if either value is an object, return an error
        if (isObject() || other.isObject()) {
            return geode::Err("Cannot perform object modulo");
        }

        // if either value is an array, return an error
        if (isArray() || other.isArray()) {
            return geode::Err("Cannot perform array modulo");
        }

        // if either value is a string, get a substring
        if (isString() || other.isString()) {
            auto const& str = isString() ? getString() : other.getString();
            auto const& num = isString() ? other.toInteger() : toInteger();

            auto start = num < 0 ? str.size() + num : num;
            auto count = num < 0 ? -num : str.size() - num;

            start = std::clamp<size_t>(start, 0, str.size() - 1);
            count = std::clamp<size_t>(count, 0, str.size() - start);

            return geode::Ok(Value(str.substr(start, count)));
        }

        // if either value is a float, convert both to floats and get the remainder
        if (isFloat() || other.isFloat()) {
            // if the divisor is zero, return NaN
            if (other.toFloat() == 0.0) {
                return geode::Ok(Value(std::numeric_limits<double>::quiet_NaN()));
            }
            return geode::Ok(Value(std::fmod(toFloat(), other.toFloat())));
        }

        // otherwise, convert both to integers and get the remainder
        // if the divisor is zero, return NaN
        if (other.toInteger() == 0) {
            return geode::Ok(Value(std::numeric_limits<double>::quiet_NaN()));
        }
        return geode::Ok(Value(toInteger() % other.toInteger()));
    }

    Value::Result Value::operator^(const Value& other) const noexcept {
        // if either value is null, return null
        if (isNull() || other.isNull()) {
            return geode::Ok(Value());
        }

        // if either value is an object, return an error
        if (isObject() || other.isObject()) {
            return geode::Err("Cannot perform object exponentiation");
        }

        // if either value is an array, return an error
        if (isArray() || other.isArray()) {
            return geode::Err("Cannot perform array exponentiation");
        }

        // if either value is a string, return an error
        if (isString() || other.isString()) {
            return geode::Err("Cannot perform string exponentiation");
        }

        // if either value is a float, convert both to floats and raise to the power
        if (isFloat() || other.isFloat()) {
            return geode::Ok(Value(std::pow(toFloat(), other.toFloat())));
        }

        // otherwise, convert both to integers and raise to the power
        return geode::Ok(Value(std::pow(toInteger(), other.toInteger())));
    }

    Value Value::operator==(const Value& other) const noexcept {
        // if both values are null, return true
        if (isNull() && other.isNull()) {
            return true;
        }

        // if only one value is null, return false
        if (isNull() || other.isNull()) {
            return false;
        }

        // if both values are objects, compare them
        if (isObject() && other.isObject()) {
            // fast fail if the objects have different sizes
            if (getObject().size() != other.getObject().size()) {
                return false;
            }

            // compare each key-value pair in the objects
            for (auto const& [key, value] : getObject()) {
                auto it = other.getObject().find(key);
                if (it == other.getObject().end() || it->second != value) {
                    return false;
                }
            }

            return true;
        }

        // if both values are arrays, compare them
        if (isArray() && other.isArray()) {
            auto const& array = getArray();
            auto const& otherArray = other.getArray();

            // fast fail if the arrays have different sizes
            if (array.size() != otherArray.size()) {
                return false;
            }

            // compare each element in the arrays
            for (size_t i = 0; i < array.size(); ++i) {
                if (array[i] != otherArray[i]) {
                    return false;
                }
            }

            return true;
        }

        // if both values are strings, compare them
        if (isString() && other.isString()) {
            return getString() == other.getString();
        }

        // if either value is a float, convert both to floats and compare
        if (isFloat() || other.isFloat()) {
            return toFloat() == other.toFloat();
        }

        // otherwise, convert both to integers and compare
        return toInteger() == other.toInteger();
    }

    Value Value::operator!=(const Value& other) const noexcept {
        // if both values are null, return false
        if (isNull() && other.isNull()) {
            return false;
        }

        // if only one value is null, return true
        if (isNull() || other.isNull()) {
            return true;
        }

        // if both values are objects, compare them
        if (isObject() && other.isObject()) {
            // fast fail if the objects have different sizes
            if (getObject().size() != other.getObject().size()) {
                return true;
            }

            // compare each key-value pair in the objects
            for (auto const& [key, value] : getObject()) {
                auto it = other.getObject().find(key);
                if (it == other.getObject().end() || it->second != value) {
                    return true;
                }
            }

            return false;
        }

        // if both values are arrays, compare them
        if (isArray() && other.isArray()) {
            auto const& array = getArray();
            auto const& otherArray = other.getArray();

            // fast fail if the arrays have different sizes
            if (array.size() != otherArray.size()) {
                return true;
            }

            // compare each element in the arrays
            for (size_t i = 0; i < array.size(); ++i) {
                if (array[i] != otherArray[i]) {
                    return true;
                }
            }

            return false;
        }

        // if both values are strings, compare them
        if (isString() && other.isString()) {
            return getString() != other.getString();
        }

        // if either value is a float, convert both to floats and compare
        if (isFloat() || other.isFloat()) {
            return toFloat() != other.toFloat();
        }

        // otherwise, convert both to integers and compare
        return toInteger() != other.toInteger();
    }

    Value Value::operator<(const Value& other) const noexcept {
        if (isFloat() || other.isFloat()) {
            return toFloat() < other.toFloat();
        }
        return toInteger() < other.toInteger();
    }

    Value Value::operator>(const Value& other) const noexcept {
        if (isFloat() || other.isFloat()) {
            return toFloat() > other.toFloat();
        }
        return toInteger() > other.toInteger();
    }

    Value Value::operator<=(const Value& other) const noexcept {
        if (isFloat() || other.isFloat()) {
            return toFloat() <= other.toFloat();
        }
        return toInteger() <= other.toInteger();
    }

    Value Value::operator>=(const Value& other) const noexcept {
        if (isFloat() || other.isFloat()) {
            return toFloat() >= other.toFloat();
        }
        return toInteger() >= other.toInteger();
    }

    Value Value::operator&&(const Value& other) const noexcept {
        return toBoolean() && other.toBoolean();
    }

    Value Value::operator||(const Value& other) const noexcept {
        return toBoolean() || other.toBoolean();
    }

    Value Value::operator!() const noexcept {
        return !toBoolean();
    }

    Value Value::operator-() const noexcept {
        if (isFloat()) {
            return -std::get<double>(m_data);
        }
        return -toInteger();
    }

    Value Value::at(const Value& key) const noexcept {
        switch (m_type) {
            case Type::Array: {
                auto const& array = getArray();
                auto index = key.toInteger();
                if (index < 0 || index >= static_cast<int64_t>(array.size())) {
                    return {}; // return null if index is out of bounds
                }
                return array[static_cast<size_t>(index)];
            }
            case Type::Object: {
                auto const& object = getObject();
                auto it = object.find(key.toString());
                if (it == object.end()) {
                    return {}; // return null if key is not found
                }
                return it->second;
            }
            case Type::String: {
                auto index = key.toInteger();
                if (index < 0 || index >= static_cast<int64_t>(getString().size())) {
                    return {}; // return null if index is out of bounds
                }
                return std::string(1, getString()[static_cast<size_t>(index)]);
            }
            default:
                return {}; // return null for all other types
        }
    }

    Value Value::operator->*(const Value& key) const noexcept {
        return {};
    }

    Value Value::operator[](size_t index) const noexcept {
        if (!isArray()) return {};
        auto const& array = getArray();
        if (index >= array.size()) return {};
        return array[index];
    }

    Value& Value::operator[](size_t index) noexcept {
        if (!isArray()) {
            m_type = Type::Array;
            m_data = Array();
        }

        auto& array = std::get<Array>(m_data);
        if (index >= array.size()) {
            array.resize(index + 1);
        }

        return array[index];
    }

    Value Value::operator[](std::string const& key) const noexcept {
        if (!isObject()) return {};
        auto const& object = getObject();
        auto it = object.find(key);
        if (it == object.end()) return {};
        return it->second;
    }

    Value& Value::operator[](std::string const &key) noexcept {
        if (!isObject()) {
            m_type = Type::Object;
            m_data = Object();
        }

        auto& object = std::get<Object>(m_data);
        return object[key];
    }
}
