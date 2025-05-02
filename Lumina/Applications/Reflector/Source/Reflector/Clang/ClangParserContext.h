#pragma once
#include "Containers/String.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"
#include "Reflector/TypeReflector.h"
#include "Reflector/ReflectionCore/ReflectionDatabase.h"

namespace Lumina::Reflection
{
    class FClangParserContext
    {
    public:

        FClangParserContext()
            : ParentReflectedType(nullptr)
            , LastReflectedType(nullptr)
            , Solution("")
            , Project("", "")
        {
        }

        ~FClangParserContext();
        
        void AddReflectedMacro(const FReflectionMacro& Macro);
        void AddGeneratedBodyMacro(const FReflectionMacro& Macro);
        
        bool TryFindMacroForCursor(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro);

        bool TryFindGeneratedBodyMacro(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro);

        void PushNamespace(const FString& Namespace);
        void PopNamespace();

        template<typename T>
        T* GetParentReflectedType();

        
        FReflectedType*                             ParentReflectedType;
        FReflectedType*                             LastReflectedType;

        FReflectionDatabase                         ReflectionDatabase;
        
        FString                                     ErrorMessage;
        FProjectSolution                            Solution;
        FReflectedProject                           Project;
        FReflectedHeader                            ReflectedHeader;
        
        THashMap<FName, TVector<FReflectionMacro>>  ReflectionMacros;
        THashMap<FName, TQueue<FReflectionMacro>>  GeneratedBodyMacros;
        
        TVector<FString>                            NamespaceStack;
        FString                                     CurrentNamespace;
        
    };

    template <typename T>
    T* FClangParserContext::GetParentReflectedType()
    {
        return static_cast<T*>(ParentReflectedType);
    }
}
