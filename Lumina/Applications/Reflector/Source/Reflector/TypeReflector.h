#pragma once
#include "ProjectSolution.h"
#include "ReflectedProject.h"
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina::Reflection
{
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

        bool WriteGeneratedFiles();
        
        FProjectSolution Solution;
        
        TVector<FReflectedProject> Projects;
        
    };
}
