#pragma once
#include "ObjectBase.h"
#include "ObjectHandle.h"
#include "Containers/Array.h"
#include "Core/Profiler/Profile.h"
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
            std::atomic<CObjectBase*> Object;
            std::atomic<uint32>     Generation;

            FEntry() : Object(nullptr), Generation(0) {}

            FEntry(CObjectBase* Obj, uint32_t Gen)
                : Object(Obj), Generation(Gen) {}

            FEntry(const FEntry& Other)
                : Object(Other.Object.load(std::memory_order_relaxed)),
                  Generation(Other.Generation.load(std::memory_order_relaxed))
            {}

            FEntry& operator=(const FEntry& Other)
            {
                if (this != &Other)
                {
                    Object.store(Other.Object.load(std::memory_order_relaxed),
                                 std::memory_order_relaxed);
                    Generation.store(Other.Generation.load(std::memory_order_relaxed),
                                     std::memory_order_relaxed);
                }
                return *this;
            }

            FEntry(FEntry&& Other) noexcept
                : Object(Other.Object.load(std::memory_order_relaxed)),
                  Generation(Other.Generation.load(std::memory_order_relaxed))
            {}

            FEntry& operator=(FEntry&& Other) noexcept
            {
                if (this != &Other)
                {
                    Object.store(Other.Object.load(std::memory_order_relaxed),
                                 std::memory_order_relaxed);
                    Generation.store(Other.Generation.load(std::memory_order_relaxed),
                                     std::memory_order_relaxed);
                }
                return *this;
            }
        };
    
        LUMINA_API uint32 GetNumObjectsAlive() const
        {
            return NumAlive.load(std::memory_order_relaxed);
        }
    
        FObjectHandle Allocate(CObjectBase* NewObj)
        {
            LUMINA_PROFILE_SCOPE();

            FScopeLock Lock(Mutex);
    
            uint32 Index;
            uint32 NewGen;
    
            if (!Free.empty())
            {
                Index = Free.front();
                Free.pop();
    
                NewGen = Objects[Index].Generation.load(std::memory_order_relaxed) + 1;
    
                Objects[Index].Object.store(NewObj, std::memory_order_release);
                Objects[Index].Generation.store(NewGen, std::memory_order_release);
            }
            else
            {
                Index = (uint32)Objects.size();
    
                Objects.emplace_back();
    
                Objects[Index].Object.store(NewObj, std::memory_order_release);
                Objects[Index].Generation.store(1u, std::memory_order_release);
                NewGen = 1u;
    
                HighWatermark.store(Index + 1u, std::memory_order_release);
            }
    
            NumAlive.fetch_add(1u, std::memory_order_relaxed);
            return FObjectHandle(Index, NewGen);
        }
    
        void Deallocate(FObjectHandle Handle)
        {
            LUMINA_PROFILE_SCOPE();
            
            FScopeLock Lock(Mutex);
            const uint32 index = Handle.Index;
    
            if (!IsIndexPublished(index))
                return;
    
            Objects[index].Object.store(nullptr, std::memory_order_release);
            Objects[index].Generation.fetch_add(1u, std::memory_order_acq_rel);
    
            Free.push(index);
            NumAlive.fetch_sub(1u, std::memory_order_relaxed);
        }
    
        void Deallocate(uint32 Index)
        {
            LUMINA_PROFILE_SCOPE();

            FScopeLock Lock(Mutex);
    
            if (!IsIndexPublished(Index))
            {
                return;
            }
    
            Objects[Index].Object.store(nullptr, std::memory_order_release);
            Objects[Index].Generation.fetch_add(1u, std::memory_order_acq_rel);
    
            Free.push(Index);
            NumAlive.fetch_sub(1u, std::memory_order_relaxed);
        }
        
        LUMINA_API CObjectBase* Resolve(FObjectHandle Handle) const
        {
            LUMINA_PROFILE_SCOPE();

            // Fast reject if the slot isn’t published yet.
            if (!IsIndexPublished(Handle.Index))
            {
                return nullptr;
            }
    
            const FEntry& Entry = Objects[Handle.Index];
    
            // Stable snapshot pattern:
            //   gen1 -> ptr -> gen2, then verify gen1 == gen2 == Handle.Generation and ptr != nullptr.
            const uint32 gen1 = Entry.Generation.load(std::memory_order_acquire);
            if (gen1 != Handle.Generation)
            {
                
                return nullptr;
            }
    
            CObjectBase* ptr = Entry.Object.load(std::memory_order_acquire);
            if (ptr == nullptr)
            {
                return nullptr;
            }
    
            const uint32 gen2 = Entry.Generation.load(std::memory_order_acquire);
            if (gen2 != gen1)
            {
                return nullptr;
            }
    
            return ptr;
        }
        
        LUMINA_API CObjectBase* Resolve(const CObjectBase* Object) const
        {
            LUMINA_PROFILE_SCOPE();

            const uint32 index = Object ? Object->InternalIndex : UINT32_MAX;
            if (!IsIndexPublished(index))
            {
                return nullptr;
            }
    
            // We return whatever is currently in the slot. If the caller wants
            // "still-valid?" semantics, they should pass a handle and use Resolve(handle).
            return Objects[index].Object.load(std::memory_order_acquire);
        }
    
        LUMINA_API FObjectHandle ToHandle(const CObjectBase* Object) const
        {
            LUMINA_PROFILE_SCOPE();

            if (Object == nullptr)
                return FObjectHandle();
    
            const uint32 index = Object->InternalIndex;
            if (!IsIndexPublished(index))
            {
                return FObjectHandle();
            }
    
            const FEntry& Entry = Objects[index];
    
            const uint32 gen = Entry.Generation.load(std::memory_order_acquire);
            
            return FObjectHandle(index, gen);
        }
    
    private:
        
        bool IsIndexPublished(uint32 index) const
        {
            const uint32 hw = HighWatermark.load(std::memory_order_acquire);
            return index < hw;
        }
    
    public:
        // Fixed-capacity table. Pre-sizing (optional) removes even the HighWatermark dance:
        // e.g., Objects.resize(kMaxCapacity); then HighWatermark = kMaxCapacity.
        TFixedVector<FEntry, 2024> Objects;
    
        TQueue<uint32>             Free;
    
    private:
        std::atomic<uint32>        NumAlive { 0 };
    
        // Highest initialized index + 1 that readers may legally access.
        std::atomic<uint32>        HighWatermark { 0 };
    
        // Writers only.
        FMutex                     Mutex;
    };

    extern LUMINA_API FCObjectArray         GObjectArray;
    
}
