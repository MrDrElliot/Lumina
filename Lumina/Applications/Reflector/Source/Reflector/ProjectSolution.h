#pragma once
#include <filesystem>

#include "EASTL/string.h"
#include "ReflectionCore/ReflectedProject.h"

namespace Lumina::Reflection
{
    class FProjectSolution
    {
    public:

        FProjectSolution(const std::filesystem::path& SlnPath);

        
        const eastl::string& GetPath() const { return Path; }
        const eastl::string& GetParentPath() const { return ParentPath; }
        bool DirtyProjectFiles();

    private:

        eastl::string Path;
        eastl::string ParentPath;
        eastl::vector<FReflectedProject> ReflectedProjects;
        
    };
}
