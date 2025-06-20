#pragma once
#include <clang-c/Index.h>

#include "ReflectedHeader.h"
#include "Reflector/ReflectionConfig.h"


namespace Lumina::Reflection
{
    class FReflectionMacro
    {
    public:

        FReflectionMacro() = default;
        FReflectionMacro(FReflectedHeader* ReflectedHeader, const CXCursor& Cursor, const CXSourceRange& Range, EReflectionMacro InType);


        EReflectionMacro        Type;
        eastl::string           HeaderID;
        uint32_t                LineNumber = 0;
        uint32_t                Position = -1;

        eastl::string           MacroContents;
    };
}
