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

        void AddReflectedProject(const FReflectedProject& Project) { ReflectedProjects.push_back(Project); }
        bool HasProjects() const { return !ReflectedProjects.empty(); }
        void GetProjects(eastl::vector<FReflectedProject>& Projects) const { Projects = ReflectedProjects; }
        
    private:

        eastl::string Path;
        eastl::string ParentPath;
        eastl::vector<FReflectedProject> ReflectedProjects;
        
    };
}
