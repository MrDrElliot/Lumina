#pragma once
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Threading/Thread.h"


namespace Lumina
{
    class CObjectBase;

    class FObjectNameHashBucket
    {
    public:

        void AddObject(const FName& PackageName, const FName& ObjectName, CObjectBase* Object)
        {
            FScopeLock Lock(Mutex);
            
            ObjectNameHash[PackageName][ObjectName] = Object;
        }

        void RemoveObject(const FName& PackageName, const FName& ObjectName)
        {
            FScopeLock Lock(Mutex);
            
            if (ObjectNameHash.find(PackageName) != ObjectNameHash.end())
            {
                auto& Hash = ObjectNameHash.at(PackageName);
                Hash.erase(ObjectName);
            }
        }

        CObjectBase* FindObject(const FName& PackageName, const FName& ObjectName) const
        {
            FScopeLock Lock(Mutex);
            
            auto OuterIt = ObjectNameHash.find(PackageName);
            if (OuterIt != ObjectNameHash.end())
            {
                auto InnerIt = OuterIt->second.find(ObjectName);
                if (InnerIt != OuterIt->second.end())
                {
                    return InnerIt->second;
                }
            }
            return nullptr;
        }

        void Clear()
        {
            ObjectNameHash.clear();
        }

        mutable FMutex Mutex;
        THashMap<FName, THashMap<FName, CObjectBase*>> ObjectNameHash;
    };
    
    
    extern LUMINA_API FObjectNameHashBucket ObjectNameHashBucket;
}
