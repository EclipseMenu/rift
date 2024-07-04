#include <rift/nodes/unaryop.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void UnaryOpNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value UnaryOpNode::getValue(Visitor* visitor) const {
        auto value = m_operand->getValue(visitor);
        switch (m_op) {
            case TokenType::MINUS: return -value;
            case TokenType::NOT: return !value;
            default:
                return Value::string("<invalid unary operator>");
        }
    }

}