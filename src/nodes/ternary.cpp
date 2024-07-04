#include <rift/nodes/ternary.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void TernaryNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value TernaryNode::getValue(Visitor* visitor) const {
        auto condition = m_condition->getValue(visitor);
        if (condition.getType() == Value::Type::Boolean) {
            if (condition.getBoolean()) {
                return m_trueBranch->getValue(visitor);
            } else {
                return m_falseBranch->getValue(visitor);
            }
        } else if (condition.getType() == Value::Type::Integer) {
            if (condition.getInteger()) {
                return m_trueBranch->getValue(visitor);
            } else {
                return m_falseBranch->getValue(visitor);
            }
        } else if (condition.getType() == Value::Type::Float) {
            if (condition.getFloat() != 0.0f) {
                return m_trueBranch->getValue(visitor);
            } else {
                return m_falseBranch->getValue(visitor);
            }
        } else {
            return Value::string("<invalid ternary condition>");
        }
    }

}