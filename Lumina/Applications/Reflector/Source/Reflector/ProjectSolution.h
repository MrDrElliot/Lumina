#pragma once
#include <filesystem>

#include "Containers/String.h"

namespace Lumina::Reflection
{
    class FProjectSolution
    {
    public:

        FProjectSolution(const std::filesystem::path& SlnPath);

        
        const FString& GetPath() const { return Path; }
        const FString& GetParentPath() const { return ParentPath; }

    private:

        FString Path;
        FString ParentPath;
    
    };
}
