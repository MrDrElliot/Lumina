#include "ReflectionDatabase.h"

#include <iostream>


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
        if(Type == nullptr || Type->DisplayName.empty())
        {
            return;
        }

        FStringHash NameHash = FStringHash(Type->DisplayName);
        FStringHash PathHash = FStringHash(Type->HeaderID);

        if (IsTypeRegistered(NameHash))
        {
            return;
        }
        
        eastl::vector<FReflectedType*>* TypeVector = &ReflectedTypes[PathHash];
        TypeVector->push_back(Type);
        
        TypeHashMap.insert_or_assign(NameHash, Type);
        
    }
}
