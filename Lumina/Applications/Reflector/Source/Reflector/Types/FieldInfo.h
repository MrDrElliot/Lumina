#pragma once
#include <clang-c/Index.h>
#include "EASTL/string.h"


namespace Lumina
{
    struct FFieldInfo
    {

        bool Validate(Reflection::FClangParserContext* Context) const
        {
            if (!Context->ReflectionDatabase.IsTypeRegistered(FStringHash(TypeName)))
            {
                eastl::string CurrentNamespaceScope = Context->CurrentNamespace;
                if (!CurrentNamespaceScope.empty())
                {
                    if (!Context->ReflectionDatabase.IsTypeRegistered(FStringHash(CurrentNamespaceScope + "::" + TypeName)))
                    {
                        Context->LogError("Failed to find reflected type: Class: %s | Property: %s | Type: %s. Please check the "
                                          "namespace scope, or specify the fully qualified type.", Context->ReflectedHeader.FileName.c_str(), Name.c_str(), TypeName.c_str());
                        return false;
                    }
                }
            }
            return true;
        }
        
        CXType                              Type;
        EPropertyTypeFlags                  Flags;
        eastl::string                       Name;
        eastl::string                       TypeName;
        eastl::vector<eastl::string>        PropertyFlags;
    };
}
