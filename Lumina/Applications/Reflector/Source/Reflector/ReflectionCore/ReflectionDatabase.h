#pragma once

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

        template<typename T, typename...Args>
        requires(eastl::is_base_of_v<FReflectedType, T>)
        T* CreateReflectedType(Args&&... args);


        eastl::vector<FReflectedProject>                                    ReflectedProjects;
        eastl::hash_map<eastl::string, eastl::vector<FReflectedType*>>      ReflectedTypes;
        eastl::hash_map<eastl::string, FReflectedType*>                     TypeHashMap;
        
    };


    //-------------------------------------------------------------------------------------

    template <typename T, typename ... Args>
    requires(eastl::is_base_of_v<FReflectedType, T>)
    T* FReflectionDatabase::CreateReflectedType(Args&&... args)
    {
        return new T(std::forward<Args>(args)...);
    }
}
