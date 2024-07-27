#pragma once

#include <string>
#include <utility>
#include <stdexcept>

namespace rift {

    template <typename T>
    class [[nodiscard]] Result {
    private:
        explicit Result(T value) : m_value(std::move(value)), m_error(false) {}
        Result(bool error, std::string message) : m_error(error), m_message(std::move(message)) {}

    public:
        [[nodiscard]] bool isError() const { return m_error; }
        [[nodiscard]] const std::string& getMessage() const { return m_message; }

        [[nodiscard]] T getValue() const { return m_value; }

        explicit operator bool() const { return !m_error; }

        T unwrap() const {
            if (m_error) {
                throw std::runtime_error(m_message);
            }
            return m_value;
        }

        T unwrapOr(T value) const {
            if (m_error) {
                return value;
            }
            return m_value;
        }

        static Result<T> error(std::string message) {
            return Result<T>(true, std::move(message));
        }

        static Result<T> success(T value) {
            return Result<T>(std::move(value));
        }

    private:
        T m_value;
        bool m_error;
        std::string m_message;
    };

}