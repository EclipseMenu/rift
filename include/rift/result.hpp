#pragma once

#include <string>
#include <utility>
#include <stdexcept>
#include <variant>

namespace rift {

    template <typename T>
    struct Ok {
        T value;
        Ok(T value) : value(std::move(value)) {}
    };

    struct Err {
        std::string message;
        Err(std::string message) : message(std::move(message)) {}
    };

    template <typename T>
    class [[nodiscard]] Result {
    public:
        Result(T value) : m_value(std::move(value)), m_error(false) {}
        Result(Ok<T> ok) : m_value(std::move(ok.value)), m_error(false) {}
        Result(Err err) : m_value(std::move(err.message)), m_error(true) {}

        [[nodiscard]] bool isError() const { return m_error; }
        [[nodiscard]] const std::string& getMessage() const {
            return std::get<std::string>(m_value);
        }

        [[nodiscard]] T getValue() const {
            return std::get<T>(m_value);
        }

        explicit operator bool() const { return !m_error; }
        T operator*() const { return unwrap(); }

        T unwrap() const {
            if (m_error) {
                throw std::runtime_error(getMessage());
            }
            return std::get<T>(m_value);
        }

        T unwrapOr(T value) const {
            if (m_error) {
                return value;
            }
            return std::get<T>(m_value);
        }

    private:
        std::variant<T, std::string> m_value;
        bool m_error;
    };

}
