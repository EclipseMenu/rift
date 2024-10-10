#include <rift.hpp>

#include <iostream>
#include <string>

#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"
#define YELLOW(text) COLOR_YELLOW text COLOR_RESET
#define BLUE(text) COLOR_BLUE text COLOR_RESET
#define RED(text) COLOR_RED text COLOR_RESET
#define GREEN(text) COLOR_GREEN text COLOR_RESET
#define VALUE(name, value) {name, rift::Value::from(value)}

static size_t passed = 0, failed = 0;
using VarMap = std::unordered_map<std::string, rift::Value>;

static bool TEST_CASE(const std::string& input, const std::string& expected, const VarMap& variables = {}) {
    auto result = rift::format(input, variables);
    if (result == expected) {
        std::cout << GREEN("Test passed:") << " "
            << YELLOW("'" << input << "'") << " -> "
            << BLUE("'" << result << "'\n");
        return true;
    }

    std::cout << RED("Test failed:") << " "
        << YELLOW("'" << input << "'") << " -> "
        << BLUE("'" << result << "'") << " "
        << RED("(expected: '" << expected << "')\n");
    return false;
}

static void RIFT_TEST_CASE(const std::string& input, const std::string& expected, const VarMap& variables = {}) {
    if (TEST_CASE(input, expected, variables)) {
        passed++;
    } else {
        failed++;
    }
}

int main() {
    std::cout << "Running tests..." << std::endl;
    {
        RIFT_TEST_CASE("Hello, {name}!", "Hello, World!", { VALUE("name", "World") });
        TEST_CASE("This should fail", "This should pass"); // Testing whether the test case function works

        RIFT_TEST_CASE("Ternary test: {((number + 2 * number) == 6) != true ? 'Hello' : (false ? 'impossible!' : name)}!",
                       "Ternary test: World!",
                       { VALUE("number", 2), VALUE("name", "World") });

        RIFT_TEST_CASE("{ 2 + }", "<ParseError: Expected number, string, identifier, or '(' at index 6>");
        RIFT_TEST_CASE("{snake_case_variable}", "Hi!", { VALUE("snake_case_variable", "Hi!") });
        RIFT_TEST_CASE("2 + 2 * 2 = {2 + 2 * 2}!", "2 + 2 * 2 = 6!");
        RIFT_TEST_CASE("sqrt(4): {sqrt(4)}", "sqrt(4): 2.00");
        RIFT_TEST_CASE("{0 / 0} {10 / false}", "inf inf"); // integer division by zero should not crash
        RIFT_TEST_CASE("{0.0 % 0.0} {0 % 0} {false % false}", "inf inf inf"); // same with modulo operator
        RIFT_TEST_CASE("{'=' * 5.0}", "====="); // making sure 5.0 is not interpreted as 0x40a00000
        RIFT_TEST_CASE("2 + 2 * {number} = {2 + 2 * number}!", "2 + 2 * 3 = 8!", { VALUE("number", 3) });
        RIFT_TEST_CASE("Is 2 + 2 equal to 4? {2 + 2 == 4}!", "Is 2 + 2 equal to 4? true!");
        RIFT_TEST_CASE("Is 2 + 2 equal to 4? {2 + 2 == 4 ? 'Yes' : 'No'}!", "Is 2 + 2 equal to 4? Yes!");
        RIFT_TEST_CASE("The length of 'Hello, World!' is {len('Hello, World!')}", "The length of 'Hello, World!' is 13");
        RIFT_TEST_CASE("The first character of 'Hello, World!' is {substr('Hello, World!', 0, 1)}", "The first character of 'Hello, World!' is H");
        RIFT_TEST_CASE("TO UPPER: {toUpper('hello, world!')}", "TO UPPER: HELLO, WORLD!");
        RIFT_TEST_CASE("to lower: {toLower('HELLO, WORLD!')}", "to lower: hello, world!");
        RIFT_TEST_CASE("Trim whitespace: {trim('  Hello, World!  ')}", "Trim whitespace: Hello, World!");
        RIFT_TEST_CASE("Replacing: {replace('Hello, World!', 'World', 'Universe')}", "Replacing: Hello, Universe!");
        RIFT_TEST_CASE("{monthName} {day}{ day > 3 || day < 21 ? 'th' : day % 10 == 1 ? 'st' : day % 10 == 2 ? 'nd' : day % 10 == 3 ? 'rd' : 'th' }, {year}",
             "October 10th, 2024",
             { VALUE("monthName", "October"), VALUE("day", 10), VALUE("year", 2024) });
        RIFT_TEST_CASE("{monthName} {day}{ordinal(day)}, {year}", "October 10th, 2024",
             { VALUE("monthName", "October"), VALUE("day", 10), VALUE("year", 2024) });
        RIFT_TEST_CASE(R"({'escaped \' quote'} {'single " quote'} {"double ' quote "})", "escaped ' quote single \" quote double ' quote ");
        RIFT_TEST_CASE("Hello!\\nNewline:{'\\n'}123", "Hello!\nNewline:\n123");
        RIFT_TEST_CASE("{null ? 'true' : 'false'}", "false");
        RIFT_TEST_CASE("{(true || null) == true}", "true");
        RIFT_TEST_CASE("{true ?? 'cool'}{false ?? 'not cool'}", "cool");
    }

    // Show the results
    std::cout << "\nResults:\n";
    size_t total = passed + failed;
    std::cout << "Tests passed: " << passed << " / " << total << std::endl;
    std::cout << "Tests failed: " << failed << " / " << total << std::endl;

    return failed == 0 ? 0 : 1;
}
