#include <rift.hpp>
#include <rift/lexer.hpp>
#include <rift/visitor.hpp>

#include <rift/nodes/segment.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/value.hpp>

#include <iostream>
#include "rift/parser.hpp"

namespace rift {

    std::string Script::run(const std::unordered_map<std::string, Value>& variables) {
        Visitor visitor(this, &variables);
        return visitor.evaluate();
    }

    Result<Script*> compile(const std::string& script) {
        Lexer lexer(script);
        Parser parser(lexer);

        auto root = parser.parse();
        if (!root) {
            std::stringstream ss;
            ss << "<ParseError: " << root.getMessage() << ">";
            return Result<Script*>::error(ss.str());
        }

        auto* s = new Script;
        s->m_nodes.push_back(std::unique_ptr<Node>(root.getValue()));
        return Result<Script*>::success(s);
    }

    std::string format(const std::string& script, const std::unordered_map<std::string, Value>& variables) {
        auto res = compile(script);
        if (!res) return res.getMessage();

        auto* s = res.getValue();
        auto result = s->run(variables);
        delete s;

        return result;
    }

}