#pragma once
#ifndef RIFT_SCRIPT_HPP
#define RIFT_SCRIPT_HPP

#include <memory>

#include "errors/runtime.hpp"
#include "value.hpp"
#include "nodes/node.hpp"

#include <Geode/Result.hpp>

namespace rift {

    using RunResult = geode::Result<std::string, RuntimeError>;
    using EvalResult = geode::Result<Value, RuntimeError>;

    class Script {
    public:
        explicit Script(std::unique_ptr<Node> root) noexcept : m_root(std::move(root)) {}
        Script(Script const&) = delete;
        Script(Script&&) = delete;

        [[nodiscard]] RunResult run(Object const& variables = {}) const noexcept;
        [[nodiscard]] EvalResult eval(Object const& variables = {}) const noexcept;

        [[nodiscard]] std::string toDebugString() const noexcept {
            return m_root->toDebugString();
        }

    private:
        std::unique_ptr<Node> m_root;
    };

}

#endif // RIFT_SCRIPT_HPP