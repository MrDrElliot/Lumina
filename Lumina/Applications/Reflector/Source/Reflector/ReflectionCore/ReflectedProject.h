#pragma once
#include "ReflectedHeader.h"
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina::Reflection
{
    class FReflectedProject
    {
    public:

        FReflectedProject(const FString& SlnPath, const FString& ProjectPath);

        bool Parse();
        
        FString Name;
        FString Path;
        FString SolutionPath;
        FString ParentPath;
        TVector<FReflectedHeader> Headers;
    
    };
}
