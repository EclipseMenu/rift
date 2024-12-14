#pragma once
#ifndef RIFT_CONFIG_HPP
#define RIFT_CONFIG_HPP

#include "value.hpp"
#include "util.hpp"

#include <span>
#include <string>
#include <functional>
#include <unordered_map>
#include <tuple>

#include <fmt/format.h>

namespace rift {

    /// @brief Can be int64_t, double, bool, std::string, Array, or Object.
    template<typename T>
    concept ValueConvertible = std::same_as<T, int64_t>
                            || std::same_as<T, double>
                            || std::same_as<T, bool>
                            || std::same_as<T, std::string>
                            || std::same_as<T, Array>
                            || std::same_as<T, Object>;

    using RuntimeFuncResult = geode::Result<Value>;
    using RuntimeFunction = std::function<RuntimeFuncResult(std::span<Value const>)>;

    /// @brief Global configuration for the Rift library.
    class Config {
        Config();
        Config(Config const&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config const&) = delete;

    public:
        /// @brief Returns the global configuration instance.
        static Config& get() noexcept;

        /// @brief Sets a global variable.
        /// @param name the name of the variable
        /// @param value the value of the variable
        void setGlobal(std::string const& name, Value value) noexcept {
            m_globals[name] = std::move(value);
        }

        /// @brief Retrieve global variables object.
        Object const& globals() const noexcept { return m_globals; }

        /// @brief Add a function to the global configuration.
        /// @param name the name of the function
        /// @param function the function to add, must follow this signature:
        /// <code>geode::Result<Value>(std::span<Value const>)</code>
        void registerFunction(std::string const& name, RuntimeFunction&& function) noexcept {
            m_functions[name] = std::move(function);
        }

        /// @brief Retrieve a function by name.
        /// @param name the name of the function
        /// @return a pointer to the function, or nullptr if not found
        RuntimeFunction const* getFunction(std::string const& name) const noexcept {
            if (auto it = m_functions.find(name); it != m_functions.end()) {
                return &it->second;
            }
            return nullptr;
        }

    private:
        template <size_t I, typename T, typename... Args>
        static geode::Result<std::tuple<T, Args...>> unwrapArgsImpl(std::span<Value const> args) {
            if constexpr (sizeof...(Args) > 0) {
                auto rest = unwrapArgsImpl<I + 1, Args...>(args);
                if (rest.isErr()) {
                    return geode::Err(rest.unwrapErr());
                }
                auto canUnwrap = args[I].is<T>();
                if (!canUnwrap) {
                    return geode::Err("Argument type mismatch");
                }
                return geode::Ok(std::tuple_cat(std::tuple<T>{args[I].to<T>()}, std::move(rest.unwrap())));
            } else {
                return geode::Ok(std::tuple<T>(args[I].to<T>()));
            }
        }

        template <std::size_t I>
        static geode::Result<std::tuple<>> unwrapArgsImpl(std::span<const Value>) {
            return geode::Ok(std::tuple());
        }

        template <typename... Args>
        static geode::Result<std::tuple<Args...>> unwrapArgs(std::span<Value const> args) {
            if constexpr (sizeof...(Args) != 0) {
                if (args.size() != sizeof...(Args)) {
                    return geode::Err("Argument count mismatch");
                }
                return unwrapArgsImpl<0, Args...>(args);
            } else {
                return geode::Ok(std::tuple<Args...>());
            }
        }

    public:
        /// @brief Constructs a wrapper around a function pointer and adds it to the configuration.
        /// Handles unwrapping arguments and wrapping the return value.
        /// @tparam Ret the return type of the function
        /// @tparam Args the argument types of the function
        /// @param name the name of the function
        /// @param func the function pointer
        template <typename Ret, typename... Args>
        void makeFunction(std::string const& name, Ret(*func)(Args...)) noexcept {
            m_functions[name] = [func](std::span<Value const> args) -> RuntimeFuncResult {
                // Unwrap the arguments with deduced types
                auto res = unwrapArgs<Args...>(args);
                if (res.isErr()) {
                    return geode::Err(res.unwrapErr());
                }

                // Apply the unwrapped arguments to the lambda function
                auto ret = std::apply(func, res.unwrap());

                // Convert the result to the correct type
                if constexpr (std::same_as<Ret, RuntimeFuncResult>) {
                    return ret; // Already a Result
                } else if constexpr (std::same_as<Ret, Value>) {
                    return geode::Ok(std::move(ret)); // Forwards the Value
                } else if constexpr (std::same_as<Ret, void>) {
                    return geode::Ok(Value()); // Return null
                } else {
                    return geode::Ok(Value(ret)); // Convert to Value
                }
            };
        }

    private:
        Object m_globals;
        std::unordered_map<std::string, RuntimeFunction> m_functions;
    };

}

#endif // RIFT_CONFIG_HPP