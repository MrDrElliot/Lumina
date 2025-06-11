#pragma once

#include <iostream>

#include "StringHash.h"
#include "EASTL/hash_map.h"
#include "Reflector/TypeReflector.h"
#include "Reflector/Types/ReflectedType.h"

namespace Lumina::Reflection
{
    class FReflectionDatabase
    {
    public:

        FReflectionDatabase() = default;
        ~FReflectionDatabase();

        void AddReflectedProject(const FReflectedProject& Project);
        void AddReflectedType(FReflectedType* Type);

        bool IsTypeRegistered(const FStringHash& Str) { return TypeHashMap.find(Str) != TypeHashMap.end(); }
        
        template<typename T>
        requires(eastl::is_base_of_v<FReflectedType, T>)
        T* GetOrCreateReflectedType(const FStringHash& TypeName);


        eastl::vector<FReflectedProject>                                    ReflectedProjects;
        eastl::hash_map<FStringHash, eastl::vector<FReflectedType*>>        ReflectedTypes;
        eastl::hash_map<FStringHash, FReflectedType*>                       TypeHashMap;
        
    };


    //-------------------------------------------------------------------------------------

    template <typename T>
    requires(eastl::is_base_of_v<FReflectedType, T>)
    T* FReflectionDatabase::GetOrCreateReflectedType(const FStringHash& TypeName)
    {
        T* ReturnValue = nullptr;
        if (IsTypeRegistered(TypeName))
        {
            ReturnValue = (T*)TypeHashMap.at(TypeName);
        }
        else
        {
            ReturnValue = new T();
        }

        ReturnValue->DisplayName = TypeName.c_str();

        return ReturnValue;
    }
}
