#pragma once
#include "ProjectSolution.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "ReflectionCore/ReflectedProject.h"

namespace Lumina::Reflection
{
    class FClangParser;

    class FTypeReflector
    {
    public:

        FTypeReflector(const FString& SolutionPath);

        /** Gathers all reflectable projects within a solution. */
        bool ParseSolution();

        /** Deletes all previously generated files */
        bool Clean();

        /** Builds generated files from reflected projects */
        bool Build();

    private:

        bool WriteGeneratedFiles(const FClangParser& Parser);
        
        FProjectSolution Solution;
        TVector<FReflectedProject> Projects;
        
    };
}
