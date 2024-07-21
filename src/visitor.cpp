#include <rift/visitor.hpp>

#include <rift/nodes/binaryop.hpp>
#include <rift/nodes/functioncall.hpp>
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

    Value Visitor::getVariable(const std::string& name) const {
        auto it = m_variables->find(name);
        if (it != m_variables->end()) {
            return it->second;
        } else {
            return Value::string("null");
        }
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
        auto it = m_variables->find(node->getName());
        if (it != m_variables->end()) {
            write(it->second.toString());
        } else {
            write("null");
        }
    }

    void Visitor::visit(ValueNode* node) {
        write(node->getValue().toString());
    }

    void Visitor::visit(UnaryOpNode* node) {
        auto value = node->getValue(this);
        write(value.toString());
    }

    void Visitor::visit(BinaryOpNode* node) {
        auto value = node->getValue(this);
        write(value.toString());
    }

    void Visitor::visit(FunctionCallNode* node) {
        auto value = node->getValue(this);
        write(value.toString());
    }

    void Visitor::visit(TernaryNode* node) {
        auto value = node->getValue(this);
        write(value.toString());
    }

    std::string Visitor::evaluate() {
        for (auto& node : m_script->m_nodes) {
            node->accept(this);
        }

        return getOutput();
    }

}