#include <rift/script.hpp>
#include <rift/visitor.hpp>

namespace rift {

    RunResult Script::run(Object const &variables) const noexcept {
        auto result = eval(variables);
        if (result.isErr()) {
            return geode::Err(std::move(result.unwrapErr()));
        }
        return geode::Ok(std::move(result.unwrap().toString()));
    }

    EvalResult Script::eval(Object const &variables) const noexcept {
        Visitor visitor(variables);
        auto result = visitor.visit(*m_root);
        if (result.isErr()) {
            return geode::Err(std::move(result.unwrapErr()));
        }
        return geode::Ok(std::move(result.unwrap()));
    }

}
