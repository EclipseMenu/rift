#pragma once
#include <functional>
#include <optional>
#include <unordered_map>
#include <span>

#include "value.hpp"

/// @brief The namespace for configuring the Rift interpreter
namespace rift::config {

    using RuntimeFunc = std::function<Value(std::span<Value>)>; // Function signature for runtime functions
    using RuntimeFuncMap = std::unordered_map<std::string, RuntimeFunc>; // Map of runtime functions

    /// @brief Get the map of runtime functions
    const RuntimeFuncMap& getRuntimeFunctions();

    /// @brief Add a runtime function to the map
    void addRuntimeFunction(const std::string& name, const RuntimeFunc& func);

}

namespace rift {
    /// @brief Get an argument from a span of values
    template <typename T>
        std::optional<T> getArgument(std::span<Value> args, size_t index) {
        if (args.size() <= index) {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<T, std::string>) {
            if (!args[index].isString()) {
                return std::nullopt;
            }
            return args[index].toString();
        } else if constexpr (std::is_same_v<T, int>) {
            if (args[index].isInteger()) {
                return args[index].getInteger();
            }
            if (args[index].isFloat()) {
                return static_cast<int>(args[index].getFloat());
            }
            return std::nullopt;
        } else if constexpr (std::is_same_v<T, float>) {
            if (args[index].isFloat()) {
                return args[index].getFloat();
            }
            if (args[index].isInteger()) {
                return static_cast<float>(args[index].getInteger());
            }
            return std::nullopt;
        } else if constexpr (std::is_same_v<T, bool>) {
            if (!args[index].isBoolean()) {
                return std::nullopt;
            }
            return args[index].getBoolean();
        }

        return std::nullopt;
    }
}