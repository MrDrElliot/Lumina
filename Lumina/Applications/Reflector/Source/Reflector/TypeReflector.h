#pragma once
#include "ProjectSolution.h"
#include "ReflectionCore/ReflectedProject.h"

namespace Lumina::Reflection
{
    class FClangParser;

    class FTypeReflector
    {
    public:

        FTypeReflector(const eastl::string& SolutionPath);

        /** Gathers all reflectable projects within a solution. */
        bool ParseSolution();

        /** Deletes all previously generated files */
        bool Clean();

        /** Builds the reflection database */
        bool Build(FClangParser& Parser);

        /** Creates and generates reflection code files */
        bool Generate(FClangParser& Parser);

    private:

        bool WriteGeneratedFiles(const FClangParser& Parser);
        
        FProjectSolution Solution;
        eastl::vector<FReflectedProject> Projects;
        
    };
}
