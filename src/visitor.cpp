#include <rift/visitor.hpp>

#include <rift/nodes/segment.hpp>
#include <rift/nodes/identifier.hpp>

namespace rift {

    void Visitor::write(const std::string& text) {
        m_output << text;
    }

    std::string Visitor::getOutput() const {
        return m_output.str();
    }

    void Visitor::visit(Node* node) {
        node->accept(this);
    }

    void Visitor::visit(SegmentNode* node) {
        write(node->getText());
    }

    void Visitor::visit(IdentifierNode* node) {
        auto it = m_script->m_variables.find(node->getName());
        if (it != m_script->m_variables.end()) {
            auto type = it->second.getType();
            if (type == Value::Type::String) {
                write(it->second.getString());
            } else if (type == Value::Type::Integer) {
                write(std::to_string(it->second.getInteger()));
            } else if (type == Value::Type::Float) {
                write(std::to_string(it->second.getFloat()));
            } else if (type == Value::Type::Boolean) {
                write(it->second.getBoolean() ? "true" : "false");
            }

            return;
        }

        write("null");
    }

    std::string Visitor::evaluate() {
        for (auto& node : m_script->m_nodes) {
            node->accept(this);
        }

        return getOutput();
    }

}