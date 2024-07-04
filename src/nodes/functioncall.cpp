#include <rift/nodes/functioncall.hpp>
#include <rift/visitor.hpp>
#include <rift/nodes/identifier.hpp>

#include <functional>
#include <random>

namespace rift {

    namespace builtins {
        Value len(const std::vector<Value>& args) {
            if (args.size() != 1) {
                return Value::string("<error: len requires 1 argument>");
            }
            const auto& value = args[0];
            return Value::integer(static_cast<int>(value.toString().size()));
        }

        Value substr(const std::vector<Value>& args) {
            if (args.size() != 3) {
                return Value::string("<error: substr requires 3 arguments>");
            }
            const auto& value = args[0];
            const auto& start = args[1];
            const auto& end = args[2];

            if (!value.isString() || !start.isInteger() || !end.isInteger()) {
                return Value::string("<error: substr requires string, integer, integer>");
            }

            return Value::string(value.toString().substr(start.getInteger(), end.getInteger()));
        }

        Value toUpper(const std::vector<Value>& args) {
            if (args.size() != 1) {
                return Value::string("<error: toUpper requires 1 argument>");
            }
            const auto& value = args[0];
            if (!value.isString()) {
                return Value::string("<error: toUpper requires string>");
            }

            std::string result = value.toString();
            for (char& c : result) {
                c = static_cast<char>(toupper(c));
            }

            return Value::string(result);
        }

        Value toLower(const std::vector<Value>& args) {
            if (args.size() != 1) {
                return Value::string("<error: toLower requires 1 argument>");
            }
            const auto& value = args[0];
            if (!value.isString()) {
                return Value::string("<error: toLower requires string>");
            }

            std::string result = value.toString();
            for (char& c : result) {
                c = static_cast<char>(tolower(c));
            }

            return Value::string(result);
        }

        Value trim(const std::vector<Value>& args) {
            if (args.size() != 1) {
                return Value::string("<error: trim requires 1 argument>");
            }
            const auto& value = args[0];
            if (!value.isString()) {
                return Value::string("<error: trim requires string>");
            }

            std::string result = value.toString();
            result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
            result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);

            return Value::string(result);
        }

        Value replace(const std::vector<Value>& args) {
            if (args.size() != 3) {
                return Value::string("<error: replace requires 3 arguments>");
            }
            const auto& value = args[0];
            const auto& from = args[1];
            const auto& to = args[2];

            if (!value.isString() || !from.isString() || !to.isString()) {
                return Value::string("<error: replace requires string, string, string>");
            }

            std::string result = value.toString();
            size_t start_pos = 0;
            while ((start_pos = result.find(from.toString(), start_pos)) != std::string::npos) {
                result.replace(start_pos, from.toString().length(), to.toString());
                start_pos += to.toString().length();
            }

            return Value::string(result);
        }

        Value random(const std::vector<Value>& args) {
            if (args.size() != 2) {
                return Value::string("<error: random requires 2 arguments>");
            }
            const auto& min = args[0];
            const auto& max = args[1];

            if (!min.isInteger() || !max.isInteger()) {
                return Value::string("<error: random requires integer, integer>");
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min.getInteger(), max.getInteger());
            return Value::integer(dis(gen));
        }
    }


    std::function<Value(const std::vector<Value>&)> findFunction(const std::string& name) {
        static const std::unordered_map<std::string, std::function<Value(const std::vector<Value>&)>> functions = {
            {"len", builtins::len},
            {"substr", builtins::substr},
            {"toUpper", builtins::toUpper},
            {"toLower", builtins::toLower},
            {"trim", builtins::trim},
            {"replace", builtins::replace},
            {"random", builtins::random},
        };

        auto it = functions.find(name);
        if (it == functions.end()) {
            return nullptr;
        }

        return it->second;
    }

    void FunctionCallNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value FunctionCallNode::getValue(Visitor* visitor) const {
        if (m_name->getType() != Type::Identifier) {
            return Value::string("<error: function name is not an identifier>");
        }
        auto funcName = reinterpret_cast<const IdentifierNode*>(m_name)->getName();
        auto function = findFunction(funcName);
        if (!function) {
            return Value::string("<error: unknown function '" + funcName + "'>");
        }

        std::vector<Value> arguments;
        arguments.reserve(m_arguments.size());
        for (auto* arg : m_arguments) {
            arguments.push_back(arg->getValue(visitor));
        }

        return function(arguments);
    }

}