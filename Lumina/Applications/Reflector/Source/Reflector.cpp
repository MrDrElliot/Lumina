

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

    eastl::string SolutionToReflect;

    std::filesystem::path slnPath;

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(2);

    std::cout << "===============================================\n";
    std::cout << "Lumina Reflection Tool\n";
    std::cout << "===============================================\n\n";
    
    if (argc >= 2)
    {
        slnPath = argv[1];

        if (!std::filesystem::exists(slnPath))
        {
            std::cerr << "Provided path does not exist: " << slnPath.generic_string().c_str() << "\n";
            return 1;
        }

        std::cout << "Reflecting Solution: " << slnPath.generic_string().c_str() << "\n";
        SolutionToReflect = slnPath.generic_string().c_str();
    }
    else
    {
        const char* LuminaDirectory = std::getenv("LUMINA_DIR");
        if (!LuminaDirectory)
        {
            std::cerr << "Usage: " << (argc > 0 ? argv[0] : "Reflector.exe") 
                      << " <path_to_solution_or_project_directory>\n";
            std::cerr << "Or set LUMINA_DIR environment variable to point to Lumina root.\n";
            return 1;
        }

        SolutionToReflect = LuminaDirectory;
        SolutionToReflect += "\\Lumina.sln";

        if (!std::filesystem::exists(SolutionToReflect.c_str()))
        {
            std::cerr << "Failed to find Lumina.sln at: " << SolutionToReflect.c_str() << "\n";
            return 1;
        }
    }

    
    if (SolutionToReflect.empty() || std::filesystem::path(SolutionToReflect.c_str()).extension() != ".sln")
    {
        std::cout << "[WARNING] Specified path does not exist, or is not a solution file: " << slnPath.string() << "\n";
        return 1;
    }

    Lumina::Reflection::FTypeReflector TypeReflector(SolutionToReflect);

    double parseTime = 0.0;
    {
        std::cout << "[Reflection] Starting Solution Parsing... " << SolutionToReflect.c_str() << "\n";
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
