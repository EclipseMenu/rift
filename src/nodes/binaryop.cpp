#include <rift/nodes/binaryop.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void BinaryOpNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value BinaryOpNode::getValue(Visitor* visitor) const {
        auto left = m_lhs->getValue(visitor);
        auto right = m_rhs->getValue(visitor);
        switch (m_op) {
            case TokenType::PLUS: return left + right;
            case TokenType::MINUS: return left - right;
            case TokenType::STAR: return left * right;
            case TokenType::SLASH: return left / right;
            case TokenType::PERCENT: return left % right;
            case TokenType::CARET: return left ^ right;
            case TokenType::AND: return left && right;
            case TokenType::OR: return left || right;
            case TokenType::EQUAL_EQUAL: return left == right;
            case TokenType::NOT_EQUAL: return left != right;
            case TokenType::LESS: return left < right;
            case TokenType::LESS_EQUAL: return left <= right;
            case TokenType::GREATER: return left > right;
            case TokenType::GREATER_EQUAL: return left >= right;
            default:
                return Value::string("<invalid binary operator>");
        }
    }

}