#pragma once
#ifndef RIFT_RUNTIME_ERROR_HPP
#define RIFT_RUNTIME_ERROR_HPP

#include <fmt/format.h>

namespace rift {

    /// @brief Represents an error that occurred during compilation (lexing/parsing).
    class [[nodiscard]] RuntimeError {
    public:
        RuntimeError(std::string message, size_t index, size_t endIndex) noexcept :
            m_message(std::move(message)), m_index(index), m_endIndex(endIndex) {}

        /// @brief Returns the error message.
        [[nodiscard]] std::string_view message() const noexcept {
            return m_message;
        }

        /// @brief Returns the index in source where the error occurred.
        [[nodiscard]] size_t index() const noexcept {
            return m_index;
        }

        /// @brief Returns a pretty message with a pointer to the error location.
        [[nodiscard]] std::string prettyPrint(std::string_view source) const noexcept {
            int length = std::max<int>(1, m_endIndex - m_index - 1);
            return fmt::format("{}\n{}\n{:>{}}", m_message, source, std::string(length, '^'), m_endIndex - 1);
        }

    private:
        std::string m_message;
        size_t m_index, m_endIndex;
    };

}

#endif // RIFT_RUNTIME_ERROR_HPP