#include <rift.hpp>

#include <iostream>
#include <string>

int main() {
    // Compile the script
    auto compiled = rift::compile("Test #{number}, Hello {name}!");

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
    std::cout << result << std::endl; // Test #420, Hello World!
}