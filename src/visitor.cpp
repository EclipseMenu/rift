#include <rift/visitor.hpp>

#include <rift/nodes/binaryop.hpp>
// #include <rift/nodes/functioncall.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/root.hpp>
#include <rift/nodes/segment.hpp>
#include <rift/nodes/ternary.hpp>
#include <rift/nodes/unaryop.hpp>
#include <rift/nodes/value.hpp>

namespace rift {

    void Visitor::write(const std::string& text) {
        m_output << text;
    }

    std::string getValueString(const Value& value) {
        if (value.getType() == Value::Type::String) {
            return value.getString();
        } else if (value.getType() == Value::Type::Integer) {
            return std::to_string(value.getInteger());
        } else if (value.getType() == Value::Type::Float) {
            return std::to_string(value.getFloat());
        } else if (value.getType() == Value::Type::Boolean) {
            return value.getBoolean() ? "true" : "false";
        }

        return "null";
    }

    std::string Visitor::getOutput() const {
        return m_output.str();
    }

    void Visitor::visit(Node* node) {
        node->accept(this);
    }

    void Visitor::visit(RootNode* node) {
        for (auto& child : node->getChildren()) {
            child->accept(this);
        }
    }

    void Visitor::visit(SegmentNode* node) {
        write(node->getText());
    }

    void Visitor::visit(IdentifierNode* node) {
        auto it = m_script->m_variables.find(node->getName());
        if (it != m_script->m_variables.end()) {
            auto type = it->second.getType();
            write(getValueString(it->second));
        } else {
            write("null");
        }
    }

    void Visitor::visit(ValueNode* node) {
        write(getValueString(node->getValue()));
    }

    void Visitor::visit(UnaryOpNode* node) {
        node->print(m_output);
    }

    void Visitor::visit(BinaryOpNode* node) {
        node->print(m_output);
    }

    void Visitor::visit(FunctionCallNode* node) {
        // node->print(m_output);
    }

    void Visitor::visit(TernaryNode* node) {
        node->print(m_output);
    }

    std::string Visitor::evaluate() {
        for (auto& node : m_script->m_nodes) {
            node->accept(this);
        }

        return getOutput();
    }

}