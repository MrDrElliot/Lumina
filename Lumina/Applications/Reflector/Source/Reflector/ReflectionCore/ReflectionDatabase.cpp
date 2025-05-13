#include "ReflectionDatabase.h"

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
        
        eastl::vector<FReflectedType*>* TypeVector = &ReflectedTypes[Type->HeaderID];
        TypeVector->push_back(Type);
        
        TypeHashMap.insert_or_assign(Type->ID, Type);
    }
}
