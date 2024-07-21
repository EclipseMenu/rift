#include <rift.hpp>
#include <rift/lexer.hpp>
#include <rift/visitor.hpp>

#include <rift/nodes/segment.hpp>
#include <rift/nodes/identifier.hpp>
#include <rift/nodes/value.hpp>

#include <iostream>
#include "rift/parser.hpp"

namespace rift {

    Script::~Script() {
        for (auto* node : m_nodes) {
            delete node;
        }
    }

    std::string Script::run(const std::unordered_map<std::string, Value>& variables) {
        Visitor visitor(this, &variables);
        return visitor.evaluate();
    }

    Script* compile(const std::string& script) {
        Lexer lexer(script);
        Parser parser(lexer);

        auto* s = new Script();
        auto* root = parser.parse();
        if (!root) {
            //std::cout << "Error: " << parser.getError() << std::endl;
            delete s;
            return nullptr;
        }

        s->m_nodes.push_back(root);

        return s;
    }

    std::string format(const std::string& script, const std::unordered_map<std::string, Value>& variables) {
        auto* s = compile(script);
        if (!s) {
            delete s;
            return "<error>";
        }

        for (const auto& [name, value] : variables) {
            s->setVariable(name, value);
        }

        auto result = s->run();
        delete s;

        return result;
    }



}