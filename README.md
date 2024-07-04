# RIFT
*Runtime Interpreted Formatter*  
Quick and dirty runtime-based string interpolation library for C++.

> **Note:** This library is still in development and is not yet ready for production use.

## What is RIFT?
RIFT is a simple, lightweight, and easy-to-use runtime interpreted string formatting library.
It is designed to be used for easier string interpolation in C++. It allows you to perform basic
calculations and string manipulations directly in the string itself:  
```cpp
#include <iostream>
#include <rift.hpp>

int main() {
    std::cout << rift::format("Test #{number * 2}, Hello {name}!", {
        {"number", rift::Value::from(210)},
        {"name", rift::Value::from("World")}
    }) << std::endl; // Prints "Test #420, Hello World!"
    return 0;
}
```

## How does it work?
Under the hood, RIFT uses a simple lexer and parser to tokenize and parse the input string.
Each `{ ... }` block is parsed as an expression, which is then evaluated at runtime. The result
is then converted to a string and inserted back into the original string.  
You can provide any variables you want (only integers, floats, bools and strings are supported) to the
script compiler, which will then be used to evaluate the expressions.

## How do I use it?
To use RIFT, simply include the `rift.hpp` header file in your project. 
You can then use the `rift::format` for a quick format, or use the full syntax like so:
```cpp
#include <iostream>
#include <rift.hpp>

int main() {
    auto* script = rift::compile("Test #{number * 2}, Hello {name}!");
    
    if (script == nullptr) {
        std::cerr << "Failed to compile script!" << std::endl;
        return 1;
    }
    
    script->setVariable("number", 210);
    script->setVariable("name", "World");
    
    std::cout << script->run() << std::endl; // Prints "Test #420, Hello World!"
    return 0;
}
```

## Why should I use RIFT over other libraries?
RIFT is designed as "Runtime" and "Turing complete" string interpolation library. This means that you can
perform any calculations and string manipulations directly in the string itself, having to provide only the
format string and the variables. This makes it useful in cases, where the user can provide their own format strings,
and you want to allow them to do some basic calculations in the string itself.

## Building
RIFT is a static library which can be built using CMake. To include the library in your project, simply add the
following to your `CMakeLists.txt`:
```cmake
add_subdirectory(path/to/rift)
target_link_libraries(your_target PRIVATE rift)
```

## License
RIFT is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.