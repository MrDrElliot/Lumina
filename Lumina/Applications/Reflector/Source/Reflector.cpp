

#include <iostream>
#include <chrono>
#include <filesystem>
#include <cstdlib>

#include "StringHash.h"
#include "Reflector/TypeReflector.h"
#include "Reflector/Clang/ClangParser.h"


int main(int argc, char* argv[])
{
    Lumina::FStringHash::Initialize();
    
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

    Lumina::Reflection::FTypeReflector TypeReflector(LuminaEditor);

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
    
    Lumina::Reflection::FClangParser Parser;
    
    double buildTime = 0.0;
    {
        std::cout << "[Reflection] Starting Build...\n";
        auto start = std::chrono::high_resolution_clock::now();

        TypeReflector.Build(Parser);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        buildTime = duration.count();

        std::cout << "[Reflection] Build completed in " << buildTime << " seconds.\n";
    }

    std::cout << "\n";

    double GenTime = 0.0;
    {
        std::cout << "[Reflection] Starting Code Generation...\n";
        auto start = std::chrono::high_resolution_clock::now();

        TypeReflector.Generate(Parser);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        GenTime = duration.count();

        std::cout << "[Reflection] Code Generation completed in " << GenTime << " seconds.\n";
    }

    double BumpTime = 0.0;
    {
        std::cout << "[Reflection] Bumping Project Files...\n";
        auto start = std::chrono::high_resolution_clock::now();
        
        TypeReflector.Bump();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        BumpTime = duration.count();

        std::cout << "[Reflection] Project file bump completed in " << BumpTime << " seconds.\n";

    }
    
    std::cout << "\n";
    
    std::cout << "===============================================\n";
    std::cout << "Total Time: " << (parseTime + buildTime + GenTime + BumpTime) << " seconds\n";
    std::cout << "===============================================\n";

    Lumina::FStringHash::Shutdown();
    
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
