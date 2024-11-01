#include <rift/nodes/execute.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void ExecuteNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value ExecuteNode::getValue(Visitor* visitor) const {
        auto value = m_child->getValue(visitor);
        if (value.isString()) {
            return Value::from(rift::format(value.getString(), *visitor->getVariables()));
        }
        return value;
    }
}
