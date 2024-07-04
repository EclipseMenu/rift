#include <rift/nodes/identifier.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void IdentifierNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value IdentifierNode::getValue(Visitor* visitor) const {
        if (m_name == "true") {
            return Value::boolean(true);
        } else if (m_name == "false") {
            return Value::boolean(false);
        }
        return visitor->getVariable(m_name);
    }

}
