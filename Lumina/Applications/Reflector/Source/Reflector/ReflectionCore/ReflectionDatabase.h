#pragma once

#include "Core/Templates/Forward.h"
#include "Memory/Memory.h"
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

        template<typename T, typename...Args>
        requires(eastl::is_base_of_v<FReflectedType, T>)
        T* CreateReflectedType(Args&&... args);


        TVector<FReflectedProject>                  ReflectedProjects;
        THashMap<FName, TVector<FReflectedType*>>   ReflectedTypes;
        THashMap<FName, FReflectedType*>            TypeHashMap;
        
    };


    //-------------------------------------------------------------------------------------

    template <typename T, typename ... Args>
    requires(eastl::is_base_of_v<FReflectedType, T>)
    T* FReflectionDatabase::CreateReflectedType(Args&&... args)
    {
        return FMemory::New<T>(TForward<Args>(args)...);
    }
}
