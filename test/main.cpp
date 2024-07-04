#include <rift.hpp>

#include <iostream>
#include <string>

int main() {
    // Compile the script
    std::string script = "Scary ternary expression test: {((2 + 2 * 2) == 6) == true ? 'Hello' : (false ? 'impossible!' : 'yep')}!";
    auto compiled = rift::compile(script);

    std::cout << "Input: " << script << std::endl << std::endl;

    // Check if the script is valid
    if (!compiled) {
        std::cerr << "Failed to compile the script!" << std::endl;
        return 1;
    }

    // Set some variables
    compiled->setVariable("number", 420);
    compiled->setVariable("name", "World");

    // Run the script
    auto result = compiled->run();

    // Print the result
    std::cout << "Output: " << std::endl;
    std::cout << result << std::endl;
}

