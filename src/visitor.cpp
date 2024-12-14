#include <rift.hpp>
#include <rift/visitor.hpp>

#include <rift/nodes/segment.hpp>
#include <rift/nodes/value.hpp>

namespace rift {

    VisitorResult Visitor::visit(Node const& node) const noexcept {
        switch (node.type()) {
            case Node::Type::Segment:
                return geode::Ok(static_cast<SegmentNode const&>(node).value());
            case Node::Type::Root:
                return visit(static_cast<RootNode const&>(node));
            case Node::Type::Identifier:
                return visit(static_cast<IdentifierNode const&>(node));
            case Node::Type::Value:
                return geode::Ok(static_cast<ValueNode const&>(node).value());
            case Node::Type::Indexer:
                return visit(static_cast<IndexerNode const&>(node));
            case Node::Type::Accessor:
                return visit(static_cast<AccessorNode const&>(node));
            case Node::Type::Binary:
                return visit(static_cast<BinaryNode const&>(node));
            case Node::Type::Call:
                return visit(static_cast<CallNode const&>(node));
            case Node::Type::Ternary:
                return visit(static_cast<TernaryNode const&>(node));
            case Node::Type::Unary:
                return visit(static_cast<UnaryNode const&>(node));
            default:
                return node.error(fmt::format("RuntimeError: Unknown node type '{}'", node.type()));
        }
    }

    VisitorResult Visitor::visit(RootNode const& node) const noexcept {
        std::string result;
        for (auto const& child : node.nodes()) {
            auto res = visit(*child);
            if (res.isErr()) {
                return res;
            }

            result += res.unwrap().toString();
        }
        return geode::Ok(Value(result));
    }

    VisitorResult Visitor::visit(IdentifierNode const& node) const noexcept {
        // find the variable in the object
        if (auto it = m_variables.get().find(node.name()); it != m_variables.get().end()) {
            return geode::Ok(it->second);
        }

        // find the variable in the builtins
        auto const& builtins = Config::get().globals();
        if (auto it = builtins.find(node.name()); it != builtins.end()) {
            return geode::Ok(it->second);
        }

        // return null if the variable is not found
        return geode::Ok(Value());
    }

    VisitorResult Visitor::visit(BinaryNode const& node) const noexcept {
        auto lhs = visit(*node.lhs());
        if (lhs.isErr()) {
            return lhs;
        }

        auto rhs = visit(*node.rhs());
        if (rhs.isErr()) {
            return rhs;
        }

#define CASE(Type, op) \
    case TokenType::Type: { \
        return geode::Ok(lhs.unwrap() op rhs.unwrap()); \
    }
#define CASE_UNWRAP(Type, op) \
    case TokenType::Type: { \
        auto res = lhs.unwrap() op rhs.unwrap(); \
        if (res.isErr()) { \
            return node.error(fmt::format("RuntimeError: {}", res.unwrapErr())); \
        } \
        return geode::Ok(res.unwrap()); \
    }


        switch (node.op()) {
            // Math operators
            CASE_UNWRAP(PLUS, +)
            CASE_UNWRAP(MINUS, -)
            CASE_UNWRAP(STAR, *)
            CASE_UNWRAP(SLASH, /)
            CASE_UNWRAP(PERCENT, %)
            CASE_UNWRAP(CARET, ^)

            // Comparison operators
            CASE(EQUAL_EQUAL, ==)
            CASE(NOT_EQUAL, !=)
            CASE(LESS, <)
            CASE(GREATER, >)
            CASE(LESS_EQUAL, <=)
            CASE(GREATER_EQUAL, >=)

            // Logical operators
            CASE(AND, &&)
            CASE(OR, ||)

            default:
                return node.error("RuntimeError: Unknown binary operator");
        }
    }

    VisitorResult Visitor::visit(UnaryNode const& node) const noexcept {
        auto res = visit(node.value());
        if (res.isErr()) {
            return res;
        }

        switch (node.op()) {
            case TokenType::PLUS:
                return res;
            case TokenType::MINUS:
                return geode::Ok(-res.unwrap());
            case TokenType::NOT:
                return geode::Ok(!res.unwrap());
            case TokenType::DOLLAR: {
                auto val = res.unwrap().toString();
                auto fmtRes = rift::format(val, m_variables);
                if (fmtRes.isErr()) {
                    return node.error(fmt::format("SubExpressionError: {}", fmtRes.unwrapErr().message()));
                }
                return geode::Ok(Value(fmtRes.unwrap()));
            }
            default:
                return node.error("RuntimeError: Unknown unary operator");
        }
    }

    VisitorResult Visitor::visit(TernaryNode const& node) const noexcept {
        auto condition = visit(node.cond());
        if (condition.isErr()) {
            return condition;
        }

        if (condition.unwrap().toBoolean()) {
            return visit(node.trueBranch());
        }

        if (node.hasFalseBranch()) {
            return visit(node.falseBranch());
        }

        return geode::Ok(Value(""));
    }

    VisitorResult Visitor::visit(CallNode const& node) const noexcept {
        std::string name;
        if (node.node()->type() == Node::Type::Identifier) {
            name = static_cast<IdentifierNode const&>(*node.node()).name();
        } else {
            auto func = visit(*node.node());
            if (func.isErr()) {
                return func;
            }
            name = func.unwrap().toString();
        }

        auto const* runtimeFunc = Config::get().getFunction(name);
        if (!runtimeFunc) {
            return node.error(fmt::format("RuntimeError: Function '{}' not found", name));
        }

        auto args = std::vector<Value>{};
        args.reserve(node.numArgs());
        for (auto const& arg : node.args()) {
            auto res = visit(*arg);
            if (res.isErr()) {
                return res;
            }
            args.push_back(res.unwrap());
        }

        auto res = (*runtimeFunc)(args);
        if (res.isErr()) {
            return node.error(fmt::format("RuntimeError: {}", res.unwrapErr()));
        }

        return geode::Ok(res.unwrap());
    }

    VisitorResult Visitor::visit(AccessorNode const& node) const noexcept {
        auto obj = visit(*node.node());
        if (obj.isErr()) {
            return obj;
        }
        return geode::Ok(std::move(obj.unwrap()[node.name()]));
    }

    VisitorResult Visitor::visit(IndexerNode const& node) const noexcept {
        auto obj = visit(*node.node());
        if (obj.isErr()) {
            return obj;
        }

        auto key = visit(*node.index());
        if (key.isErr()) {
            return key;
        }

        return geode::Ok(obj.unwrap().at(key.unwrap()));
    }
}
