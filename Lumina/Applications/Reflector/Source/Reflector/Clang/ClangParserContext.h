#pragma once
#include "Containers/String.h"
#include "Reflector/ReflectionMacro.h"
#include "Reflector/TypeReflector.h"

namespace Lumina::Reflection
{
    class FClangParserContext
    {
    public:

        FClangParserContext() = default;

        void AddReflectedMacro(const FReflectionMacro& Macro);
        bool GetMacroForType(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro);

        void PushNamespace(const FString& Namespace);
        void Popnamespace();
        
        
        FString                                     ErrorMessage;
        FString                                     SolutionPath;
        FReflectedHeader                            ReflectedHeader;
        
        THashMap<FName, TVector<FReflectionMacro>>  ReflectionMacros;
        
        TVector<FString>                            NamespaceStack;
        FString                                     CurrentNamespace;
        
    };
}
