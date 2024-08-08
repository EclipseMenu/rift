#include <rift.hpp>
#include <rift/lexer.hpp>
#include <rift/visitor.hpp>

#include <rift/nodes/segment.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/value.hpp>

#include <iostream>
#include <fmt/format.h>
#include "rift/parser.hpp"

namespace rift {

    std::string Script::run(const std::unordered_map<std::string, Value>& variables) {
        Visitor visitor(this, &variables);
        return visitor.evaluate();
    }

    Result<Script*> compile(std::string_view script) {
        Lexer lexer(script);
        Parser parser(lexer);

        auto root = parser.parse();
        if (!root) {
            return Result<Script*>::error(fmt::format("<ParseError: {}>", root.getMessage()));
        }

        auto* s = new Script;
        s->m_nodes.push_back(std::unique_ptr<Node>(root.getValue()));
        return Result<Script*>::success(s);
    }

    std::string format(std::string_view script, const std::unordered_map<std::string, Value>& variables) {
        auto res = compile(script);
        if (!res) return res.getMessage();

        auto* s = res.getValue();
        auto result = s->run(variables);
        delete s;

        return result;
    }

}