#include "ReflectionDatabase.h"

#include "Memory/Memory.h"

namespace Lumina::Reflection
{
    void FReflectionDatabase::AddReflectedType(FReflectedType* Type)
    {
        if (TypeHashMap.find(Type->ID) == TypeHashMap.end())
        {
            ReflectedTypes.emplace_back(FMemory::Move(Type));
            TypeHashMap.insert_or_assign(Type->ID, Type);
        }
        else
        {
            FMemory::Delete(Type);
            LOG_WARN("Attempted to register a type that's already been registered");
        }
    }
}
