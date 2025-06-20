#pragma once
#include "ReflectedHeader.h"
#include "StringHash.h"

#include "EASTL/hash_map.h"

namespace Lumina::Reflection
{
    class FReflectedProject
    {
    public:

        FReflectedProject(const eastl::string& SlnPath, const eastl::string& ProjectPath);

        bool Parse();
        
        eastl::string                                   Name;
        eastl::string                                   Path;
        eastl::string                                   SolutionPath;
        eastl::string                                   ParentPath;
        eastl::vector<FReflectedHeader>                 Headers;
        eastl::hash_map<FStringHash, FReflectedHeader>  HeaderHashMap;
    
    };
}
