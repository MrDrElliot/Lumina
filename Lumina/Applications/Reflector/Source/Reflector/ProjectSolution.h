#pragma once
#include <filesystem>

#include "EASTL/string.h"

namespace Lumina::Reflection
{
    class FProjectSolution
    {
    public:

        FProjectSolution(const std::filesystem::path& SlnPath);

        
        const eastl::string& GetPath() const { return Path; }
        const eastl::string& GetParentPath() const { return ParentPath; }

    private:

        eastl::string Path;
        eastl::string ParentPath;
    
    };
}
