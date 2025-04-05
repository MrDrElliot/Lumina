#pragma once
#include "Reflector/ReflectionCore/ReflectionDatabase.h"

namespace Lumina::Reflection
{
    class FCodeGenerator
    {
    public:

        FCodeGenerator(const FProjectSolution& SlnPath, const FReflectionDatabase& Database);


        void GenerateCodeForSolution();
        void GenerateCodeForProject(const FReflectedProject& Project);

        void GenerateReflectionCodeForHeader(const FReflectedHeader& Header);

        void SetProject(const FReflectedProject& Project) { CurrentProject = Project; }

    private:

        void GenerateCodeHeader(std::stringstream& SS, const FReflectedHeader& Header);


    private:

        
        FReflectedProject           CurrentProject;
        FProjectSolution            Solution;
        const FReflectionDatabase*  ReflectionDatabase;
        
        
    };
}
