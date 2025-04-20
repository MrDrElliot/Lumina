#include "ReflectionDatabase.h"

#include "Memory/Memory.h"

namespace Lumina::Reflection
{
    FReflectionDatabase::~FReflectionDatabase()
    {
        for (const auto& Pair : ReflectedTypes)
        {
            for (FReflectedType* Type : Pair.second)
            {
                FMemory::Delete(Type);
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
        if(Type == nullptr || !Type->ID.IsValid())
        {
            LOG_WARN("Attempted to register a null type");
            return;
        }
        
        TVector<FReflectedType*>* TypeVector = &ReflectedTypes[Type->HeaderID];
        Assert(TypeVector != nullptr);
        TypeVector->push_back(Type);
        
        TypeHashMap.insert_or_assign(Type->ID, Type);
    }
}
