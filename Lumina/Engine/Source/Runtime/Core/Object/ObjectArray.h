#pragma once
#include "ObjectBase.h"
#include "ObjectHandle.h"
#include "Containers/Array.h"
#include "Core/Threading/Thread.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CObjectBase;
}

namespace Lumina
{
    class FCObjectArray
    {
    public:
        
        struct FEntry
        {
            CObjectBase*    Object = nullptr;
            uint32          Generation;
        };

        LUMINA_API uint32 GetNumObjectsAlive() const
        {
            return NumAlive;
        }
        
        FObjectHandle Allocate(CObjectBase* NewObj)
        {
            FScopeLock Lock(Mutex);
            uint32 Index;

            if (!Free.empty())
            {
                Index = Free.front();
                Free.pop();
                Objects[Index].Object = NewObj;
                Objects[Index].Generation++;
            }
            else
            {
                Index = (uint32)Objects.size();
                Objects.emplace_back(NewObj, 1);
            }
            NumAlive++;
            return FObjectHandle(Index, Objects[Index].Generation);
        }

        void Deallocate(FObjectHandle Handle)
        {
            FScopeLock Lock(Mutex);
            if (Handle.Index >= Objects.size())
                return;

            Objects[Handle.Index].Object = nullptr;
            Objects[Handle.Index].Generation++;
            Free.push(Handle.Index);
            NumAlive--;
        }

        void Deallocate(uint32 Index)
        {
            FScopeLock Lock(Mutex);
            if (Index >= Objects.size())
                return;

            Objects[Index].Object = nullptr;
            Objects[Index].Generation++;
            Free.push(Index);
            NumAlive--;
        }

        LUMINA_API CObjectBase* Resolve(FObjectHandle Handle)
        {
            FScopeLock Lock(Mutex);
            
            if (Handle.Index >= Objects.size())
                return nullptr;

            const FEntry& Entry = Objects[Handle.Index];
            
            if (Entry.Generation != Handle.Generation)
                return nullptr;

            return Entry.Object;
        }

        LUMINA_API CObjectBase* Resolve(const CObjectBase* Object)
        {
            FScopeLock Lock(Mutex);

            const FEntry& Entry = Objects[Object->InternalIndex];
            return Entry.Object;
        }

        LUMINA_API FObjectHandle ToHandle(const CObjectBase* Object)
        {
            FScopeLock Lock(Mutex);
            if (Object == nullptr)
            {
                return FObjectHandle();
            }

            const FEntry& Entry = Objects[Object->InternalIndex];
            FObjectHandle Handle;
            Handle.Index = Object->InternalIndex;
            Handle.Generation = Entry.Generation;

            return Handle;
        }
        

        TFixedVector<FEntry, 2024>          Objects;
        TQueue<uint32>                      Free;
        uint32                              NumAlive = 0;
        FMutex                              Mutex;
    };

    extern LUMINA_API FCObjectArray         GObjectArray;
    
}
