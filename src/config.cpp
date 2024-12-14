#include <rift/config.hpp>

#include <numbers>
#include <limits>
#include <random>

namespace rift {

    namespace builtins {
        // Cast functions
        int64_t intCast(int64_t value) noexcept { return value; }
        double floatCast(double value) noexcept { return value; }
        std::string strCast(std::string value) noexcept { return std::move(value); }

        // String functions
        int64_t length(std::string str) noexcept {
            return str.size();
        }

        std::string toUpper(std::string str) noexcept {
            for (auto& c : str) {
                c = std::toupper(c);
            }
            return str;
        }

        std::string toLower(std::string str) noexcept {
            for (auto& c : str) {
                c = std::tolower(c);
            }
            return str;
        }

        std::string substrImpl(const std::string &str, int64_t start, int64_t count) noexcept {
            // If string is empty, return empty string
            if (str.empty()) {
                return "";
            }

            // Handle negative start index and count
            if (start < 0) {
                start = str.size() + start;
            }
            if (count < 0) {
                count = str.size() + count - start;
            }

            // Make sure the start index is within bounds
            if (start < 0) {
                start = 0;
            } else if (start >= str.size()) {
                return "";
            }

            // Make sure the count is within bounds
            if (count < 0) {
                count = 0;
            } else if (count > str.size() - start) {
                count = str.size() - start;
            }

            // Return the substring
            return str.substr(start, count);
        }

        RuntimeFuncResult substr(std::span<Value const> args) noexcept {
            if (args.size() != 3 && args.size() != 2) {
                return geode::Err("Expected 2 or 3 arguments");
            }
            if (!args[0].is<std::string>() || !args[1].is<int64_t>() || (args.size() == 3 && !args[2].is<int64_t>())) {
                return geode::Err("Argument type mismatch");
            }
            return geode::Ok(substrImpl(
                args[0].to<std::string>(),
                args[1].to<int64_t>(),
                args.size() == 3 ? args[2].to<int64_t>() : std::numeric_limits<int64_t>::max()
            ));
        }

        std::string trim(std::string str) noexcept {
            // Find the first non-whitespace character
            auto start = str.find_first_not_of(" \t\n\r\f\v");
            if (start == std::string::npos) {
                return "";
            }

            // Find the last non-whitespace character
            auto end = str.find_last_not_of(" \t\n\r\f\v");

            // Return the trimmed string
            return str.substr(start, end - start + 1);
        }

        std::string replace(std::string str, std::string from, std::string to) noexcept {
            size_t start = 0;
            while ((start = str.find(from, start)) != std::string::npos) {
                str.replace(start, from.length(), to);
                start += to.length();
            }
            return str;
        }

        int64_t find(std::string str, std::string substr) noexcept {
            return str.find(substr);
        }

        int64_t round(double value) noexcept {
            return static_cast<int64_t>(std::round(value));
        }

        int64_t floor(double value) noexcept {
            return static_cast<int64_t>(std::floor(value));
        }

        int64_t ceil(double value) noexcept {
            return static_cast<int64_t>(std::ceil(value));
        }

        std::string precision(double value, int64_t precision) noexcept {
            return fmt::format("{:.{}f}", value, precision);
        }

        std::string ordinal(int64_t value) noexcept {
            if (value > 3 && value < 21) {
                return "th";
            }
            switch (value % 10) {
                case 1: return "st";
                case 2: return "nd";
                case 3: return "rd";
                default: return "th";
            }
        }

        std::string duration(double time) noexcept {
            auto hours = static_cast<int>(time / 3600);
            auto minutes = static_cast<int>(time / 60) % 60;
            auto seconds = static_cast<int>(time) % 60;
            auto millis = static_cast<int>(time * 1000) % 1000;

            if (hours > 0) {
                return fmt::format("{}:{:02}:{:02}.{:03}", hours, minutes, seconds, millis);
            }
            if (minutes > 0) {
                return fmt::format("{}:{:02}.{:03}", minutes, seconds, millis);
            }
            return fmt::format("{}.{:03}", seconds, millis);
        }

        RuntimeFuncResult middlePad(std::span<Value const> args) noexcept {
            if (args.size() != 2 && args.size() != 3) {
                return geode::Err("Expected 2 or 3 arguments");
            }

            auto str = args[0].to<std::string>();
            auto width = args[1].to<int64_t>();
            auto pad = args.size() == 3 ? args[2].to<std::string>() : " ";
            if (pad.size() != 1) {
                return geode::Err("Expected a single character for padding");
            }

            if (str.size() >= static_cast<size_t>(width)) {
                return geode::Ok(str);
            }

            auto padSize = width - str.size();
            auto leftPad = padSize / 2;
            auto rightPad = padSize - leftPad;
            return geode::Ok(std::string(leftPad, pad[0]) + str + std::string(rightPad, pad[0]));
        }

        RuntimeFuncResult leftPad(std::span<Value const> args) noexcept {
            if (args.size() != 2 && args.size() != 3) {
                return geode::Err("Expected 2 or 3 arguments");
            }

            auto str = args[0].to<std::string>();
            auto width = args[1].to<int64_t>();
            auto pad = args.size() == 3 ? args[2].to<std::string>() : " ";
            if (pad.size() != 1) {
                return geode::Err("Expected a single character for padding");
            }

            if (str.size() >= static_cast<size_t>(width)) {
                return geode::Ok(str);
            }

            auto padSize = width - str.size();
            return geode::Ok(std::string(padSize, pad[0]) + str);
        }

        RuntimeFuncResult rightPad(std::span<Value const> args) noexcept {
            if (args.size() != 2 && args.size() != 3) {
                return geode::Err("Expected 2 or 3 arguments");
            }

            auto str = args[0].to<std::string>();
            auto width = args[1].to<int64_t>();
            auto pad = args.size() == 3 ? args[2].to<std::string>() : " ";
            if (pad.size() != 1) {
                return geode::Err("Expected a single character for padding");
            }

            if (str.size() >= static_cast<size_t>(width)) {
                return geode::Ok(str);
            }

            auto padSize = width - str.size();
            return geode::Ok(str + std::string(padSize, pad[0]));
        }

        RuntimeFuncResult min(std::span<Value const> args) noexcept {
            if (args.empty()) {
                return geode::Err("Expected at least one argument");
            }

            auto min = std::move(args[0]);
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] < min) {
                    min = std::move(args[i]);
                }
            }

            return geode::Ok(std::move(min));
        }

        RuntimeFuncResult max(std::span<Value const> args) noexcept {
            if (args.empty()) {
                return geode::Err("Expected at least one argument");
            }

            auto max = std::move(args[0]);
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] > max) {
                    max = std::move(args[i]);
                }
            }

            return geode::Ok(std::move(max));
        }

        RuntimeFuncResult sum(std::span<Value const> args) noexcept {
            if (args.empty()) {
                return geode::Err("Expected at least one argument");
            }

            double sum = 0;
            for (size_t i = 0; i < args.size(); ++i) {
                sum += args[i].toFloat();
            }

            return geode::Ok(sum);
        }

        RuntimeFuncResult avg(std::span<Value const> args) noexcept {
            if (args.empty()) {
                return geode::Err("Expected at least one argument");
            }

            auto sum = args[0].toFloat();
            for (size_t i = 1; i < args.size(); ++i) {
                sum = sum + args[i].toFloat();
            }

            return geode::Ok(sum / args.size());
        }

        int64_t randomInt(int64_t min, int64_t max) noexcept {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution dist(min, max);
            return dist(gen);
        }

        double randomFloat(double min, double max) noexcept {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution dist(min, max);
            return dist(gen);
        }

        RuntimeFuncResult random(std::span<Value const> args) noexcept {
            if (args.size() != 2) {
                return geode::Err("Expected 2 arguments");
            }

            // if any argument is a float, return a float
            if (args[0].isFloat() || args[1].isFloat()) {
                return geode::Ok(randomFloat(args[0].toFloat(), args[1].toFloat()));
            }
            return geode::Ok(randomInt(args[0].toInteger(), args[1].toInteger()));
        }

    }

    Config::Config() {
        // Register built-in variables
        m_globals = {
            { "true", true },
            { "false", false },
            { "null", {} },
            { "PI", std::numbers::pi },
            { "E", std::numbers::e },
            { "inf", std::numeric_limits<double>::infinity() },
            { "nan", std::numeric_limits<double>::quiet_NaN() },
        };

        // Register built-in functions
        makeFunction("int", builtins::intCast);
        makeFunction("float", builtins::floatCast);
        makeFunction("str", builtins::strCast);
        makeFunction("len", builtins::length);
        makeFunction("toUpper", builtins::toUpper);
        makeFunction("toLower", builtins::toLower);
        registerFunction("substr", builtins::substr);
        makeFunction("trim", builtins::trim);
        makeFunction("replace", builtins::replace);
        makeFunction("find", builtins::find);
        makeFunction("round", builtins::round);
        makeFunction("floor", builtins::floor);
        makeFunction("ceil", builtins::ceil);
        makeFunction("precision", builtins::precision);
        makeFunction("ordinal", builtins::ordinal);
        makeFunction("duration", builtins::duration);
        makeFunction("randomInt", builtins::randomInt);
        makeFunction("randomFloat", builtins::randomFloat);
        registerFunction("middlePad", builtins::middlePad);
        registerFunction("leftPad", builtins::leftPad);
        registerFunction("rightPad", builtins::rightPad);
        registerFunction("min", builtins::min);
        registerFunction("max", builtins::max);
        registerFunction("sum", builtins::sum);
        registerFunction("avg", builtins::avg);
        registerFunction("random", builtins::random);
        makeFunction<double, double>("sqrt", std::sqrt);
        makeFunction<double, double>("cbrt", std::cbrt);
        makeFunction<double, double>("abs", std::abs);
        makeFunction<double, double>("sin", std::sin);
        makeFunction<double, double>("cos", std::cos);
        makeFunction<double, double>("tan", std::tan);
        makeFunction<double, double>("asin", std::asin);
        makeFunction<double, double>("acos", std::acos);
        makeFunction<double, double>("atan", std::atan);
        makeFunction<double, double>("sinh", std::sinh);
        makeFunction<double, double>("cosh", std::cosh);
        makeFunction<double, double>("tanh", std::tanh);
        makeFunction<double, double>("asinh", std::asinh);
        makeFunction<double, double>("acosh", std::acosh);
        makeFunction<double, double>("atanh", std::atanh);
        makeFunction<double, double>("exp", std::exp);
        makeFunction<double, double>("log", std::log);
        makeFunction<double, double>("log10", std::log10);
        makeFunction<double, double, double>("pow", std::pow);
        makeFunction<double, double, double>("hypot", std::hypot);
        makeFunction<double, double, double>("atan2", std::atan2);

        // Function aliases
        makeFunction("ord", builtins::ordinal);
        registerFunction("lpad", builtins::leftPad);
        registerFunction("mpad", builtins::middlePad);
        registerFunction("rpad", builtins::rightPad);
        makeFunction("prec", builtins::precision);
        registerFunction("rand", builtins::random);
    }

    Config& Config::get() noexcept {
        static Config instance;
        return instance;
    }

}
