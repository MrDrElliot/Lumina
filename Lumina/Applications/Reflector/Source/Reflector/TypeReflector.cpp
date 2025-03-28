#include "TypeReflector.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Clang/ClangParser.h"

namespace Lumina::Reflection
{
    FTypeReflector::FTypeReflector(const FString& InSolutionPath)
        :SolutionPath(InSolutionPath)
    {
    }

    bool FTypeReflector::ParseSolution()
    {
        std::cout << "Parsing Solution" << SolutionPath.c_str() << "\n";

        std::filesystem::path CurrentPath = std::filesystem::current_path() / "Source" / "Testing";

        for (const auto& Path : std::filesystem::recursive_directory_iterator(CurrentPath))
        {
            if (Path.is_directory())
            {
                continue;
            }

            if (Path.path().extension() == ".h")
            {
                FClangParser Parser;
                Parser.Parse(Path.path().string().c_str());
            }
        }
        
        return true;

    }

    bool FTypeReflector::Clean()
    {
        return false;
    }

    bool FTypeReflector::Build()
    {
        return false;
    }

    bool FTypeReflector::WriteGeneratedFiles()
    {
        return false;
    }
}
