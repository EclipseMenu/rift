#include <rift.hpp>
#include <fmt/format.h>

#include "fmt/color.h"

static size_t TEST_COUNT = 0;
static size_t TEST_FAILED = 0;
static size_t TEST_PASSED = 0;

void RIFT_EVAL(std::string_view code, rift::Value const& expected, rift::Object const& vars = {}, bool directMode = true) {
    TEST_COUNT++;
    auto res = rift::compile(code, directMode);
    if (!res) {
        TEST_FAILED++;
        fmt::print(fmt::fg(fmt::color::orange_red), "Test failed: ");
        fmt::print(fmt::fg(fmt::color::yellow), "'{}'", code);
        fmt::print(fmt::fg(fmt::color::white), " -> ");
        fmt::print(fmt::fg(fmt::color::orange_red), "{}\n", res.unwrapErr().prettyPrint());
        return;
    }

    auto result = res.unwrap()->eval(vars);
    if (result.isErr()) {
        TEST_FAILED++;
        fmt::print(fmt::fg(fmt::color::orange_red), "Test failed: ");
        fmt::print(fmt::fg(fmt::color::yellow), "'{}'", code);
        fmt::print(fmt::fg(fmt::color::white), " -> ");
        fmt::print(fmt::fg(fmt::color::orange_red), "{}\n", result.unwrapErr().prettyPrint(code));
        return;
    }

    bool passed = false;
    if (!directMode) {
        passed = result.unwrap().toString() == expected.toString();
    } else {
        passed = result.unwrap() == expected;
    }

    if (passed) {
        TEST_PASSED++;
        fmt::print(fmt::fg(fmt::color::lime), "Test passed: ");
        fmt::print(fmt::fg(fmt::color::yellow), "'{}'", code);
        fmt::print(fmt::fg(fmt::color::white), " -> ");
        fmt::print(fmt::fg(fmt::color::cornflower_blue), "'{}'\n", result.unwrap().toString());
    } else {
        TEST_FAILED++;
        fmt::print(fmt::fg(fmt::color::orange_red), "Test failed: ");
        fmt::print(fmt::fg(fmt::color::yellow), "'{}'", code);
        fmt::print(fmt::fg(fmt::color::white), " -> ");
        fmt::print(fmt::fg(fmt::color::cornflower_blue), "'{}'", result.unwrap().toString());
        fmt::print(fmt::fg(fmt::color::orange_red), " (expected: '{}')\n", expected.toString());
    }
}

void RIFT_TEST(std::string_view code, std::string_view expected, rift::Object const& vars = {}) {
    RIFT_EVAL(code, expected, vars, false);
}

std::string myCustomFunc(std::string name) {
    return fmt::format("Hello, {}!", name);
}

int main() {
    rift::Config::get().makeFunction("myCustomFunc", myCustomFunc);

    RIFT_TEST("Hello, {name}!", "Hello, World!", {{"name", "World"}});
    RIFT_EVAL("name == 'World'", true, {{"name", "World"}});
    RIFT_TEST(
        "Ternary test: {((number + 2 * number) == 6) != true ? 'Hello' : (false ? 'impossible!' : name)}!",
        "Ternary test: World!",
        {{"number", 2}, {"name", "World"}}
    );
    RIFT_TEST("{_snake_case_variable_}", "Hi!", {{"_snake_case_variable_", "Hi!"}});
    RIFT_TEST("2 + 2 * 2 = {2 + 2 * 2}!", "2 + 2 * 2 = 6!");
    RIFT_TEST("sqrt(4): {sqrt(4)}", "sqrt(4): 2.00");
    RIFT_TEST("{0 / 0} {10 / false}", "inf inf"); // integer division by zero should not crash
    RIFT_TEST("{0.0 % 0.0} {0 % 0} {false % false}", "nan nan nan"); // same with modulo operator
    RIFT_TEST("{'=' * 5.0}", "====="); // making sure 5.0 is not interpreted as 0x40a00000
    RIFT_TEST("2 + 2 * {number} = {2 + 2 * number}!", "2 + 2 * 3 = 8!", {{"number", 3}});
    RIFT_TEST("Is 2 + 2 equal to 4? {2 + 2 == 4}!", "Is 2 + 2 equal to 4? true!");
    RIFT_TEST("Is 2 + 2 equal to 4? {2 + 2 == 4 ? 'Yes' : 'No'}!", "Is 2 + 2 equal to 4? Yes!");
    RIFT_TEST("The length of 'Hello, World!' is {len('Hello, World!')}", "The length of 'Hello, World!' is 13");
    RIFT_TEST("The first character of 'Hello, World!' is {substr('Hello, World!', 0, 1)}", "The first character of 'Hello, World!' is H");
    RIFT_TEST("TO UPPER: {toUpper('hello, world!')}", "TO UPPER: HELLO, WORLD!");
    RIFT_TEST("to lower: {toLower('HELLO, WORLD!')}", "to lower: hello, world!");
    RIFT_TEST("Trim whitespace: {trim('  Hello, World!  ')}", "Trim whitespace: Hello, World!");
    RIFT_TEST("Replacing: {replace('Hello, World!', 'World', 'Universe')}", "Replacing: Hello, Universe!");
    RIFT_TEST(
        "{monthName} {day}{ day > 3 || day < 21 ? 'th' : day % 10 == 1 ? 'st' : day % 10 == 2 ? 'nd' : day % 10 == 3 ? 'rd' : 'th' }, {year}",
        "October 10th, 2024",
        {{"monthName", "October"}, {"day", 10}, {"year", 2024}}
    );
    RIFT_TEST(
        "{monthName} {day}{ordinal(day)}, {year}",
        "October 10th, 2024",
        {{"monthName", "October"}, {"day", 10}, {"year", 2024}}
    );
    RIFT_TEST(R"({'escaped \' quote'} {'single " quote'} {"double ' quote "})", "escaped ' quote single \" quote double ' quote ");
    RIFT_TEST("Hello!\\nNewline:{'\\n'}123", "Hello!\nNewline:\n123");
    RIFT_TEST("{null ? 'true' : 'false'}", "false");
    RIFT_TEST("{(true || null) == true}", "true");
    RIFT_TEST("{true ?? 'cool'}{false ?? 'not cool'}", "cool");
    RIFT_TEST("{duration(123.456)}", "2:03.456");
    RIFT_TEST("{myCustomFunc('World')}", "Hello, World!");
    RIFT_TEST("{$'string interpolation: {2 + 2}'}", "string interpolation: 4");
    RIFT_TEST("{float('3.14')} {int('100')} {str(3.1415926)} {int('ABC')}", "3.14 100 3.14 0");
    RIFT_TEST("{precision(float('3.149268') * 1.5, 6)}", "4.723902");
    RIFT_TEST("{'*' * 5} {4 * '*'} {'*' * 3.0} {2.0 * '*'}", "***** **** *** **");
    RIFT_TEST("{!value} {!true} {!false} {!nullval}", "false false true true", {{"value", true}});
    RIFT_EVAL("--'7'", 7); // unary minus converts string to number
    RIFT_EVAL("('hello world' - 'hello ')[2] == 'r'", true); // string subtraction
    RIFT_TEST("{middlePad('#' * (progress * 4 / 10), 40, '-')} {progress}%", "----------####################---------- 50%", {{"progress", 50}});
    RIFT_TEST("{min(20, 40)}", "20");
    RIFT_EVAL("-1 * 'hello'", ""); // making sure string multiplication with negative number is empty

    fmt::println("\nResults:\nTests passed: {}/{}\nTests failed: {}/{}", TEST_PASSED, TEST_COUNT, TEST_FAILED, TEST_COUNT);
    return TEST_FAILED;
}

