#include "ReflectionDatabase.h"

#include "Memory/Memory.h"

namespace Lumina::Reflection
{
    FReflectionDatabase::~FReflectionDatabase()
    {
        while (!ReflectedTypes.empty())
        {
            FReflectedType* Type = ReflectedTypes.back();
            ReflectedTypes.pop_back();

            FMemory::Delete(Type);
        }
    }

    void FReflectionDatabase::AddReflectedType(FReflectedType* Type)
    {
        if(Type == nullptr)
        {
            LOG_WARN("Attempted to register a null type");
            return;
        }

        if(!Type->ID.IsValid())
        {
            LOG_WARN("Attempted to register a null type");
            return;
        }
        
        
        if (TypeHashMap.find(Type->ID) == TypeHashMap.end())
        {
            ReflectedTypes.emplace_back(FMemory::Move(Type));
            TypeHashMap.insert_or_assign(Type->ID, Type);
        }
        else
        {
            FMemory::Delete(Type);
            LOG_WARN("Attempted to register type ({0}) that's already been registered", Type->ID.c_str());
        }
    }
}
