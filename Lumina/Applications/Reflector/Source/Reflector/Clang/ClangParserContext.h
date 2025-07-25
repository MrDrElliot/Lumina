﻿#pragma once
#include "EASTL/hash_map.h"
#include "EASTL/queue.h"
#include "Engine/Source/Runtime/Platform/GenericPlatform.h"
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
        
        void AddReflectedMacro(FReflectionMacro&& Macro);
        void AddGeneratedBodyMacro(FReflectionMacro&& Macro);
        
        bool TryFindMacroForCursor(eastl::string HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro);

        bool TryFindGeneratedBodyMacro(eastl::string HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro);

        void LogError(char const* ErrorFormat, ...) const;
        void LogWarning(char const* ErrorFormat, ...) const;
        void FlushLogs();

        bool HasError() const { return !ErrorMessage.empty(); }


        void PushNamespace(const eastl::string& Namespace);
        void PopNamespace();

        template<typename T>
        T* GetParentReflectedType();

        
        FReflectedType*                                             ParentReflectedType;
        FReflectedType*                                             LastReflectedType;
                                                                    
        FReflectionDatabase                                         ReflectionDatabase;
        
        mutable eastl::string                                       ErrorMessage;
        mutable eastl::string                                       WarningMessage;
        
        FProjectSolution                                            Solution;
        FReflectedProject                                           Project;
        FReflectedHeader                                            ReflectedHeader;
        
        eastl::hash_map<uint64, eastl::vector<FReflectionMacro>>    ReflectionMacros;
        eastl::hash_map<uint64, eastl::queue<FReflectionMacro>>     GeneratedBodyMacros;
        
        eastl::vector<eastl::string>                                NamespaceStack;
        eastl::string                                               CurrentNamespace;
                                                                    
        uint32                                                      NumHeadersReflected = 0;

        bool                                                        bInitialPass = true;
        
    };

    template <typename T>
    T* FClangParserContext::GetParentReflectedType()
    {
        return static_cast<T*>(ParentReflectedType);
    }
}
