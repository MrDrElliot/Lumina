#include "GarbageCollector.h"

#include <tracy/Tracy.hpp>

#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"
#include "Core/Object/ObjectBase.h"
#include "Core/Profiler/Profile.h"
#include "Core/Threading/Thread.h"


namespace Lumina::GarbageCollection
{
    static TQueue<CObjectBase*> PendingDeletes;
    static FMutex DeletesLock;
    
    void AddGarbage(CObjectBase* Obj)
    {
        FScopeLock Lock(DeletesLock);

        // Object was already marked for garbage, no need to do it again.
        if (Obj->IsMarkedGarbage())
        {
            return;
        }
        
        PendingDeletes.push(Obj);
        
        Obj->SetFlag(OF_MarkedGarbage);
        Obj->OnMarkedGarbage();
    }

    void CollectGarbage()
    {
        LUMINA_PROFILE_SECTION("Garbage Collection");
        while (!PendingDeletes.empty())
        {
            CObjectBase* Obj = PendingDeletes.front();
            PendingDeletes.pop();

            Assert(Obj->IsMarkedGarbage())

            Obj->OnDestroy();
            Memory::Delete(Obj);
        }
    }
}
