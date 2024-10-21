#include <rift/nodes/functioncall.hpp>
#include <rift/visitor.hpp>
#include <rift/nodes/identifier.hpp>

#include <functional>
#include <random>
#include <fmt/format.h>
#include <cmath>
#include <cctype>
#include <optional>
#include <span>

namespace rift {

    namespace builtins {
        template <typename T>
        std::optional<T> getArgument(std::span<Value> args, size_t index) {
            if (args.size() <= index) {
                return std::nullopt;
            }

            if constexpr (std::is_same_v<T, std::string>) {
                if (!args[index].isString()) {
                    return std::nullopt;
                }
                return args[index].toString();
            } else if constexpr (std::is_same_v<T, int>) {
                if (args[index].isInteger()) {
                    return args[index].getInteger();
                }
                if (args[index].isFloat()) {
                    return static_cast<int>(args[index].getFloat());
                }
                return std::nullopt;
            } else if constexpr (std::is_same_v<T, float>) {
                if (args[index].isFloat()) {
                    return args[index].getFloat();
                }
                if (args[index].isInteger()) {
                    return static_cast<float>(args[index].getInteger());
                }
                return std::nullopt;
            } else if constexpr (std::is_same_v<T, bool>) {
                if (!args[index].isBoolean()) {
                    return std::nullopt;
                }
                return args[index].getBoolean();
            }

            return std::nullopt;
        }

        Value len(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: len requires 1 argument>");
            }
            const auto& value = args[0];
            return Value::integer(static_cast<int>(value.toString().size()));
        }

        Value substr(std::span<Value> args) {
            if (args.size() != 3) {
                return Value::string("<error: substr requires 3 arguments>");
            }
            auto value = getArgument<std::string>(args, 0);
            auto start = getArgument<int>(args, 1);
            auto end = getArgument<int>(args, 2);

            if (!value || !start || !end) {
                return Value::string("<error: substr requires string, integer, integer>");
            }

            return Value::string(value->substr(start.value(), end.value()));
        }

        Value toUpper(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: toUpper requires 1 argument>");
            }
            auto value = getArgument<std::string>(args, 0);
            if (!value) {
                return Value::string("<error: toUpper requires string>");
            }

            std::string result = value.value();
            for (char& c : result) {
                c = static_cast<char>(toupper(c));
            }

            return Value::string(result);
        }

        Value toLower(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: toLower requires 1 argument>");
            }
            auto value = getArgument<std::string>(args, 0);
            if (!value) {
                return Value::string("<error: toLower requires string>");
            }

            std::string result = value.value();
            for (char& c : result) {
                c = static_cast<char>(tolower(c));
            }

            return Value::string(result);
        }

        Value trim(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: trim requires 1 argument>");
            }
            auto value = getArgument<std::string>(args, 0);
            if (!value) {
                return Value::string("<error: trim requires string>");
            }

            std::string result = value.value();
            result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
            result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);

            return Value::string(result);
        }

        Value replace(std::span<Value> args) {
            if (args.size() != 3) {
                return Value::string("<error: replace requires 3 arguments>");
            }
            auto value = getArgument<std::string>(args, 0);
            auto from = getArgument<std::string>(args, 1);
            auto to = getArgument<std::string>(args, 2);

            if (!value || !from || !to) {
                return Value::string("<error: replace requires string, string, string>");
            }

            std::string result = value.value();
            size_t start_pos = 0;
            while ((start_pos = result.find(from.value(), start_pos)) != std::string::npos) {
                result.replace(start_pos, from.value().length(), to.value());
                start_pos += to.value().length();
            }

            return Value::string(result);
        }

        Value random(std::span<Value> args) {
            if (args.size() != 2) {
                return Value::string("<error: random requires 2 arguments>");
            }
            auto min = getArgument<int>(args, 0);
            auto max = getArgument<int>(args, 1);

            if (!min || !max) {
                return Value::string("<error: random requires integer, integer>");
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min.value(), max.value());
            return Value::integer(dis(gen));
        }

        Value round(std::span<Value> args) {
            if (args.size() != 1 && args.size() != 2) {
                return Value::string("<error: round requires 1 or 2 arguments>");
            }

            auto value = getArgument<float>(args, 0);
            if (!value) {
                return Value::string("<error: round requires float>");
            }

            if (args.size() == 1) {
                auto rounded = std::roundf(value.value());
                return Value::integer(static_cast<int>(rounded));
            }

            auto precision = getArgument<int>(args, 1);
            if (!precision) {
                return Value::string("<error: round requires float, integer>");
            }

            auto rounded = static_cast<float>(
                std::round(value.value() * std::pow(10, static_cast<float>(precision.value())))
                    / std::pow(10, static_cast<float>(precision.value()))
            );
            return Value::floating(rounded);
        }

        Value floor(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: floor requires 1 argument>");
            }

            auto value = getArgument<float>(args, 0);
            if (!value) {
                return Value::string("<error: floor requires float>");
            }

            return Value::integer(static_cast<int>(std::floor(value.value())));
        }

        Value ceil(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: ceil requires 1 argument>");
            }

            auto value = getArgument<float>(args, 0);
            if (!value) {
                return Value::string("<error: ceil requires float>");
            }

            return Value::integer(static_cast<int>(std::ceil(value.value())));
        }

        Value abs(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: abs requires 1 argument>");
            }

            const auto &value = args[0];
            if (value.isInteger()) {
                return Value::integer(std::abs(value.getInteger()));
            } else if (value.isFloat()) {
                return Value::floating(std::abs(value.getFloat()));
            }

            return Value::string("<error: abs requires integer or float>");
        }

        Value min(std::span<Value> args) {
            if (args.empty()) {
                return Value::string("<error: min requires at least 1 argument>");
            }

            const Value* min = &args[0];
            float minValue = min->toFloat();
            for (size_t i = 1; i < args.size(); ++i) {
                if (!args[i].isFloat() && !args[i].isInteger()) {
                    return Value::string("<error: min requires numbers>");
                }

                float value = args[i].toFloat();
                if (value < minValue) {
                    min = &args[i];
                    minValue = value;
                }
            }

            return *min;
        }

        Value max(std::span<Value> args) {
            if (args.empty()) {
                return Value::string("<error: max requires at least 1 argument>");
            }

            const Value* max = &args[0];
            float maxValue = max->toFloat();
            for (size_t i = 1; i < args.size(); ++i) {
                if (!args[i].isFloat() && !args[i].isInteger()) {
                    return Value::string("<error: max requires numbers>");
                }

                float value = args[i].toFloat();
                if (value > maxValue) {
                    max = &args[i];
                    maxValue = value;
                }
            }

            return *max;
        }

        Value sum(std::span<Value> args) {
            if (args.empty()) {
                return Value::string("<error: sum requires at least 1 argument>");
            }

            float sum = 0.0f;
            for (const auto& arg : args) {
                if (!arg.isFloat() && !arg.isInteger()) {
                    return Value::string("<error: sum requires numbers>");
                }

                sum += arg.toFloat();
            }

            return Value::floating(sum);
        }

        Value avg(std::span<Value> args) {
            if (args.empty()) {
                return Value::string("<error: avg requires at least 1 argument>");
            }

            float sum = 0.0f;
            for (const auto& arg : args) {
                if (!arg.isFloat() && !arg.isInteger()) {
                    return Value::string("<error: avg requires numbers>");
                }

                sum += arg.toFloat();
            }

            return Value::floating(sum / static_cast<float>(args.size()));
        }

        Value sqrt(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: sqrt requires 1 argument>");
            }

            const auto &value = args[0];
            if (!value.isFloat() && !value.isInteger()) {
                return Value::string("<error: sqrt requires number>");
            }

            return Value::floating(std::sqrt(value.toFloat()));
        }

        Value pow(std::span<Value> args) {
            if (args.size() != 2) {
                return Value::string("<error: pow requires 2 arguments>");
            }

            const auto& base = args[0];
            const auto& exponent = args[1];
            if (!base.isFloat() && !base.isInteger() && !exponent.isFloat() && !exponent.isInteger()) {
                return Value::string("<error: pow requires numbers>");
            }

            return Value::floating(std::pow(base.toFloat(), exponent.toFloat()));
        }

        Value sin(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: sin requires 1 argument>");
            }

            const auto &value = args[0];
            if (!value.isFloat() && !value.isInteger()) {
                return Value::string("<error: sin requires number>");
            }

            return Value::floating(std::sin(value.toFloat()));
        }

        Value cos(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: cos requires 1 argument>");
            }

            const auto &value = args[0];
            if (!value.isFloat() && !value.isInteger()) {
                return Value::string("<error: cos requires number>");
            }

            return Value::floating(std::cos(value.toFloat()));
        }

        Value tan(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: tan requires 1 argument>");
            }

            const auto &value = args[0];
            if (!value.isFloat() && !value.isInteger()) {
                return Value::string("<error: tan requires number>");
            }

            return Value::floating(std::tan(value.toFloat()));
        }

        Value precision(std::span<Value> args) {
            if (args.size() != 2) {
                return Value::string("<error: precision requires 2 arguments>");
            }

            auto value = getArgument<float>(args, 0);
            auto precision = getArgument<int>(args, 1);
            if (!value || !precision) {
                return Value::string("<error: precision requires float, integer>");
            }

            return Value::string(fmt::format("{:.{}f}", value.value(), precision.value()));
        }

        Value leftPad(std::span<Value> args) {
            if (args.size() != 2 && args.size() != 3) {
                return Value::string("<error: leftPad requires 2 or 3 arguments>");
            }

            auto value = getArgument<std::string>(args, 0);
            auto length = getArgument<int>(args, 1);
            if (!value || !length) {
                return Value::string("<error: leftPad requires string, integer>");
            }

            char padChar = ' ';
            if (args.size() == 3) {
                auto pad = getArgument<std::string>(args, 2);
                if (!pad || pad.value().size() != 1) {
                    return Value::string("<error: leftPad requires string, integer, string of length 1>");
                }
                padChar = pad.value()[0];
            }

            if (value.value().size() >= static_cast<size_t>(length.value())) {
                return Value::string(value.value());
            }

            return Value::string(std::string(length.value() - value.value().size(), padChar) + value.value());
        }

        Value rightPad(std::span<Value> args) {
            if (args.size() != 2 && args.size() != 3) {
                return Value::string("<error: rightPad requires 2 or 3 arguments>");
            }

            auto value = getArgument<std::string>(args, 0);
            auto length = getArgument<int>(args, 1);
            if (!value || !length) {
                return Value::string("<error: rightPad requires string, integer>");
            }

            char padChar = ' ';
            if (args.size() == 3) {
                auto pad = getArgument<std::string>(args, 2);
                if (!pad || pad.value().size() != 1) {
                    return Value::string("<error: rightPad requires string, integer, string of length 1>");
                }
                padChar = pad.value()[0];
            }

            if (value.value().size() >= static_cast<size_t>(length.value())) {
                return Value::string(value.value());
            }

            return Value::string(value.value() + std::string(length.value() - value.value().size(), padChar));
        }

        Value middlePad(std::span<Value> args) {
            if (args.size() != 2 && args.size() != 3) {
                return Value::string("<error: middlePad requires 2 or 3 arguments>");
            }

            auto value = getArgument<std::string>(args, 0);
            auto length = getArgument<int>(args, 1);
            if (!value || !length) {
                return Value::string("<error: middlePad requires string, integer>");
            }

            char padChar = ' ';
            if (args.size() == 3) {
                auto pad = getArgument<std::string>(args, 2);
                if (!pad || pad.value().size() != 1) {
                    return Value::string("<error: middlePad requires string, integer, string of length 1>");
                }
                padChar = pad.value()[0];
            }

            if (value.value().size() >= static_cast<size_t>(length.value())) {
                return Value::string(value.value());
            }

            auto padLength = static_cast<size_t>(length.value()) - value.value().size();
            auto leftPadLength = padLength / 2;
            auto rightPadLength = padLength - leftPadLength;

            return Value::string(std::string(leftPadLength, padChar) + value.value() + std::string(rightPadLength, padChar));
        }

        Value ordinal(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: ordinal requires 1 argument>");
            }

            auto value = getArgument<int>(args, 0);
            if (!value) {
                return Value::string("<error: ordinal requires integer>");
            }

            int n = value.value();
            if (n > 3 && n < 21) {
                return Value::string("th");
            }
            switch (n % 10) {
                case 1: return Value::string("st");
                case 2: return Value::string("nd");
                case 3: return Value::string("rd");
                default: return Value::string("th");
            }
        }

        Value duration(std::span<Value> args) {
            if (args.size() != 1) {
                return Value::string("<error: duration requires 1 argument>");
            }

            auto value = getArgument<float>(args, 0);
            if (!value) {
                return Value::string("<error: duration requires number>");
            }

            auto time = value.value();
            auto hours = static_cast<int>(time / 3600);
            auto minutes = static_cast<int>(time / 60);
            auto seconds = static_cast<int>(time) % 60;
            auto millis = static_cast<int>(time * 1000) % 1000;

            if (hours > 0)
                return Value::string(fmt::format("{}:{:02d}:{:02d}.{:03d}", hours, minutes, seconds, millis));
            if (minutes > 0)
                return Value::string(fmt::format("{}:{:02d}.{:03d}", minutes, seconds, millis));
            return Value::string(fmt::format("{}.{:03d}", seconds, millis));
        }
    }

    std::function<Value(std::span<Value>)> findFunction(std::string_view name) {
        static const std::unordered_map<std::string_view, std::function<Value(std::span<Value>)>> functions = {
            {"len", builtins::len},
            {"substr", builtins::substr},
            {"toUpper", builtins::toUpper},
            {"toLower", builtins::toLower},
            {"trim", builtins::trim},
            {"replace", builtins::replace},
            {"random", builtins::random},
            {"round", builtins::round},
            {"floor", builtins::floor},
            {"ceil", builtins::ceil},
            {"abs", builtins::abs},
            {"min", builtins::min},
            {"max", builtins::max},
            {"sum", builtins::sum},
            {"avg", builtins::avg},
            {"sqrt", builtins::sqrt},
            {"pow", builtins::pow},
            {"sin", builtins::sin},
            {"cos", builtins::cos},
            {"tan", builtins::tan},
            {"precision", builtins::precision},
            {"leftPad", builtins::leftPad},
            {"rightPad", builtins::rightPad},
            {"middlePad", builtins::middlePad},
            {"ordinal", builtins::ordinal},
            {"duration", builtins::duration},
            // Aliases
            {"lpad", builtins::leftPad},
            {"rpad", builtins::rightPad},
            {"mpad", builtins::middlePad},
            {"ord", builtins::ordinal},
            {"prec", builtins::precision},
            {"rand", builtins::random}
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
            return Value::string(fmt::format("<error: unknown function '{}'>", funcName));
        }

        std::vector<Value> arguments;
        arguments.reserve(m_arguments.size());
        for (auto* arg : m_arguments) {
            arguments.push_back(arg->getValue(visitor));
        }

        return function(arguments);
    }

}
