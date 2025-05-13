

#include <clang-c/Index.h>
#include <iostream>

#include "Reflector/TypeReflector.h"


using namespace Lumina;

int main(int argc, char* argv[])
{
    const char* LuminaDirectory = std::getenv("LUMINA_DIR");
    eastl::string LuminaEditor(LuminaDirectory);
    LuminaEditor += "\\Lumina.sln";
    
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision( 2 );

    std::cout << ("===============================================\n");
    std::cout << ("Lumina Reflection Tool\n");
    std::cout << ("===============================================\n");

    if (!std::filesystem::exists(LuminaEditor.c_str()))
    {
        std::cerr << "Failed to find Lumina.sln";
        return 0;
    }

    Reflection::FTypeReflector TypeReflector(LuminaEditor);

    
    {
        auto start = std::chrono::high_resolution_clock::now();

        if (!TypeReflector.ParseSolution())
        {
            return 0;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();

        TypeReflector.Build();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
    }
    
    return 0;
}


namespace eastl
{
    void AssertionFailure(const char* expression)
    {
        // You can also hook this into your engine's log or crash reporter
        std::fprintf(stderr, "EASTL Assertion Failure: %s\n", expression);
        std::abort(); // Crash the program, like assert would
    }
}
