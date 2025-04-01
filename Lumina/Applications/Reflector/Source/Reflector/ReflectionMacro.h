#pragma once
#include <clang-c/Index.h>

#include "ReflectedHeader.h"
#include "ReflectionConfig.h"



namespace Lumina::Reflection
{
    class FReflectionMacro
    {
    public:

        FReflectionMacro(FReflectedHeader* ReflectedHeader, CXCursor Cursor, CXSourceRange Range, EReflectionMacro InType);


        EReflectionMacro                Type;
        FName                           HeaderID;
        uint32                          LineNumber = 0;
        uint32                          Position = 0xFFFFFFFF;

        FString                         MacroContents;
    };
}
