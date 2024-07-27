#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "rift/result.hpp"
#include "rift/value.hpp"
#include "rift/nodes/node.hpp"

namespace rift {

    class Script {
    public:
        /// @brief Set a variable in the script.
        /// @param name The name of the variable.
        /// @param value The value of the variable.
        template<typename T>
        void setVariable(const std::string& name, T value) {
            m_variables.insert_or_assign(name, Value::from(value));
        }

        /// @brief Set a variable in the script.
        /// @param name The name of the variable.
        /// @param value The value of the variable.
        void setVariable(const std::string& name, const Value& value) {
            m_variables.insert_or_assign(name, value);
        }

        /// @brief Run the script with the specified variables.
        /// @param variables The variables to use in the script.
        /// @return The result of the script.
        std::string run(const std::unordered_map<std::string, Value>& variables);

        /// @brief Run the script.
        /// @return The result of the script.
        std::string run() { return run(m_variables); }

    private:
        std::vector<std::unique_ptr<Node>> m_nodes;
        std::unordered_map<std::string, Value> m_variables;

        friend class Visitor;
        friend Result<Script*> compile(const std::string& script);
    };

    /// @brief Compile a script.
    /// @param script The script to compile.
    Result<Script*> compile(const std::string& script);

    /// @brief Compile and run a script.
    /// @param script The script to run.
    /// @param variables The variables to use in the script.
    std::string format(const std::string& script, const std::unordered_map<std::string, Value>& variables = {});

}