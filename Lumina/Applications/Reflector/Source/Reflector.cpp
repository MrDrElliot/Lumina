

#include <clang-c/Index.h>
#include <iostream>

#include "Reflector/TypeReflector.h"


using namespace Lumina;

#include <chrono>
#include <iostream>
#include <filesystem>
#include <cstdlib> // for std::getenv

int main(int argc, char* argv[])
{
    const char* LuminaDirectory = std::getenv("LUMINA_DIR");
    if (!LuminaDirectory)
    {
        std::cerr << "LUMINA_DIR environment variable not set.\n";
        return 1;
    }

    eastl::string LuminaEditor(LuminaDirectory);
    LuminaEditor += "\\Lumina.sln";

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(2);

    std::cout << "===============================================\n";
    std::cout << "Lumina Reflection Tool\n";
    std::cout << "===============================================\n";

    std::cout << "\n";

    if (!std::filesystem::exists(LuminaEditor.c_str()))
    {
        std::cerr << "Failed to find Lumina.sln at: " << LuminaEditor.c_str() << "\n";
        return 1;
    }

    Reflection::FTypeReflector TypeReflector(LuminaEditor);

    double parseTime = 0.0;
    {
        std::cout << "[Reflection] Starting Solution Parsing...\n";
        auto start = std::chrono::high_resolution_clock::now();

        if (!TypeReflector.ParseSolution())
        {
            std::cerr << "[Reflection] Failed to parse solution.\n";
            return 1;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        parseTime = duration.count();

        std::cout << "[Reflection] Solution parsing completed in " << parseTime << " seconds.\n";
    }

    std::cout << "\n";
    
    double buildTime = 0.0;
    {
        std::cout << "[Reflection] Starting Build...\n";
        auto start = std::chrono::high_resolution_clock::now();

        TypeReflector.Build();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        buildTime = duration.count();

        std::cout << "[Reflection] Build completed in " << buildTime << " seconds.\n";
    }

    std::cout << "\n";
    
    std::cout << "===============================================\n";
    std::cout << "Total Time: " << (parseTime + buildTime) << " seconds\n";
    std::cout << "===============================================\n";

    return 0;
}

namespace eastl
{
    void AssertionFailure(const char* expression)
    {
        std::fprintf(stderr, "EASTL Assertion Failure: %s\n", expression);
        std::abort();
    }
}
