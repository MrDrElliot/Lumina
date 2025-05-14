#include "ReflectionDatabase.h"

#include <iostream>

#include "xxhash.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection
{
    FReflectionDatabase::~FReflectionDatabase()
    {
        for (const auto& Pair : ReflectedTypes)
        {
            for (FReflectedType* Type : Pair.second)
            {
                delete Type;
            }
        }

        ReflectedTypes.clear();
    }

    void FReflectionDatabase::AddReflectedProject(const FReflectedProject& Project)
    {
        ReflectedProjects.push_back(Project);
    }

    void FReflectionDatabase::AddReflectedType(FReflectedType* Type)
    {
        if(Type == nullptr || Type->ID.empty())
        {
            return;
        }

        uint64_t Hash = ClangUtils::HashString(Type->HeaderID);        
        eastl::vector<FReflectedType*>* TypeVector = &ReflectedTypes[Hash];
        TypeVector->push_back(Type);
        
        TypeHashMap.insert_or_assign(Type->ID, Type);
    }
}
