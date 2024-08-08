#pragma once

#include <rift.hpp>
#include <string>
#include <sstream>

#include "nodes/node.hpp"

namespace rift {

    /// @brief A visitor for the AST nodes.
    class Visitor {
    public:
        explicit Visitor(Script* script, const std::unordered_map<std::string, Value>* variables) :
            m_script(script), m_variables(variables) {}

        /// @brief Destruct the visitor.
        virtual ~Visitor() = default;

        /// @brief Get a variable from the script.
        /// @param name The name of the variable.
        /// @return The value of the variable.
        [[nodiscard]] Value getVariable(const std::string& name) const;

        /// @brief Visit a node.
        void visit(Node* node);

        /// @brief Visit a root node.
        void visit(class RootNode* node);

        /// @brief Visit a segment node.
        void visit(class SegmentNode* node);

        /// @brief Visit an identifier node.
        void visit(class IdentifierNode* node);

        /// @brief Visit a value node.
        void visit(class ValueNode* node);

        /// @brief Visit a unary operation node.
        void visit(class UnaryOpNode* node);

        /// @brief Visit a binary operation node.
        void visit(class BinaryOpNode* node);

        /// @brief Visit a function call node.
        void visit(class FunctionCallNode* node);

        /// @brief Visit a ternary operation node.
        void visit(class TernaryNode* node);

        /// @brief Write a string to the output.
        /// @param text The text to write.
        void write(std::string_view text);

        /// @brief Get the output.
        /// @return The output.
        [[nodiscard]] const std::string& getOutput() const;

        /// @brief Evaluate the script.
        /// @return The result of the script.
        [[nodiscard]] const std::string& evaluate();

    private:
        Script* m_script;
        std::string m_output;
        const std::unordered_map<std::string, Value>* m_variables;
    };

}