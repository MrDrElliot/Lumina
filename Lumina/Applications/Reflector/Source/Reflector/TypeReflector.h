#pragma once
#include "Containers/String.h"

namespace Lumina::Reflection
{
    class FTypeReflector
    {
    public:

        FTypeReflector(const FString& SolutionPath);

        bool ParseSolution();
        bool Clean();
        bool Build();

    private:

        bool WriteGeneratedFiles();
        
        FString SolutionPath;
    };
}
