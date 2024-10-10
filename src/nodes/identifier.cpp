#include <rift/nodes/identifier.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void IdentifierNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value IdentifierNode::getValue(Visitor* visitor) const {
        if (m_name == "true") {
            return Value::boolean(true);
        }
        if (m_name == "false") {
            return Value::boolean(false);
        }
        if (m_name == "null") {
            return Value::null();
        }
        return visitor->getVariable(m_name);
    }

}
