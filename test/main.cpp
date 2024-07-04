#include <rift.hpp>

#include <iostream>
#include <string>

int main() {
    // Compile the script
    std::string script = "Scary ternary expression test: {((number + 2 * number) == 6) != true ? 'Hello' : (false ? 'impossible!' : name)}!";
    auto compiled = rift::compile(script);

    std::cout << "Input: " << script << std::endl;

    // Check if the script is valid
    if (!compiled) {
        std::cerr << "Failed to compile the script!" << std::endl;
        return 1;
    }

    // Set some variables
    compiled->setVariable("number", 2);
    compiled->setVariable("name", "World");

    // Run the script
    auto result = compiled->run();

    // Print the result
    std::cout << "Output: " << std::endl;
    std::cout << result << std::endl; // Expected: "Scary ternary expression test: World!"
}

