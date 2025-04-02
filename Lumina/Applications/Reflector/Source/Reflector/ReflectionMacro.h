#pragma once
#include <clang-c/Index.h>

#include "Lumina.h"
#include "ReflectedHeader.h"
#include "ReflectionConfig.h"



namespace Lumina::Reflection
{
    class FReflectionMacro
    {
    public:

        FReflectionMacro() = default;
        FReflectionMacro(FReflectedHeader* ReflectedHeader, const CXCursor& Cursor, const CXSourceRange& Range, EReflectionMacro InType);


        EReflectionMacro                Type;
        FName                           HeaderID;
        uint32                          LineNumber = 0;
        uint32                          Position = INVALID_HANDLE;

        FString                         MacroContents;
    };
}
