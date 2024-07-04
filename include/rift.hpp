#pragma once

#include <string>
#include <unordered_map>
#include "rift/value.hpp"

namespace rift {

    class Script {
    public:
        /// @brief Destructor.
        ~Script();

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

        /// @brief Run the script.
        /// @return The result of the script.
        std::string run();

    private:
        std::vector<class Node*> m_nodes;
        std::unordered_map<std::string, Value> m_variables;

        friend class Visitor;
        friend Script* compile(const std::string& script);
    };

    /// @brief Compile a script.
    /// @param script The script to compile.
    Script* compile(const std::string& script);

    /// @brief Compile and run a script.
    /// @param script The script to run.
    /// @param variables The variables to use in the script.
    std::string format(const std::string& script, const std::unordered_map<std::string, Value>& variables = {});

}