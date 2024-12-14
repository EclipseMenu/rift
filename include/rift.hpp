#pragma once
#ifndef RIFT_HPP
#define RIFT_HPP

#include "rift/script.hpp"
#include "rift/value.hpp"
#include "rift/config.hpp"
#include "rift/errors/compile.hpp"

#include <Geode/Result.hpp>

namespace rift {

    using CompileResult = geode::Result<std::unique_ptr<Script>, CompileError>;
    using FormatResult = geode::Result<std::string, CompileError>;
    using EvaluateResult = geode::Result<Value, CompileError>;

    /// @brief Compiles a script from a string
    /// @param source the source code to compile
    /// @param directMode whether the script will be used in evaluation mode (no segments)
    /// @return a Result containing the compiled script if successful, otherwise a CompileError
    CompileResult compile(std::string_view source, bool directMode = false) noexcept;

    /// @brief Formats a script using the given variables
    /// @param source the source code to format
    /// @param variables the variables to use in the script
    /// @return a Result containing the formatted string if successful, otherwise a CompileError
    FormatResult format(std::string_view source, Object const& variables = {}) noexcept;

    /// @brief Evaluates a script using the given variables
    /// @param source the source code to evaluate
    /// @param variables the variables to use in the script
    /// @return a Result containing the evaluated value if successful, otherwise a CompileError
    EvaluateResult evaluate(std::string_view source, Object const& variables = {}) noexcept;

}

#endif // RIFT_HPP