#include <rift/nodes/ternary.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void TernaryNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value TernaryNode::getValue(Visitor* visitor) const {
        auto condition = m_condition->getValue(visitor);
        switch (condition.getType()) {
            case Value::Type::Boolean:
                return pickBranch(condition.getBoolean(), visitor);
            case Value::Type::Integer:
                return pickBranch(condition.getInteger(), visitor);
            case Value::Type::Float:
                return pickBranch(condition.getFloat() != 0.0f, visitor);
            case Value::Type::Null:
                return pickBranch(false, visitor);
            default:
                return Value::string("<invalid ternary condition>");
        }
    }

    Value TernaryNode::pickBranch(bool result, Visitor* visitor) const {
        if (m_onlyTrue) {
            // Return an empty string if the condition is false. (Makes the syntax more clean.)
            return result ? m_trueBranch->getValue(visitor) : Value::string("");
        }
        if (result) {
            return m_trueBranch->getValue(visitor);
        }
        return m_falseBranch->getValue(visitor);
    }
}
