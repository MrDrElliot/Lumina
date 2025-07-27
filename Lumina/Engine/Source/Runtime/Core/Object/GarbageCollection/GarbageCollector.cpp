#include "GarbageCollector.h"

#include <tracy/Tracy.hpp>

#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"
#include "Core/Object/ObjectAllocator.h"
#include "Core/Object/ObjectBase.h"
#include "Core/Profiler/Profile.h"
#include "Core/Threading/Thread.h"


namespace Lumina::GarbageCollection
{
    static TQueue<CObjectBase*> PendingDeletes;
    static FMutex DeletesLock;
    
    void AddGarbage(CObjectBase* Obj)
    {

        // Object was already marked for garbage, no need to do it again.
        if (Obj->IsMarkedGarbage())
        {
            return;
        }
        
        
        Obj->SetFlag(OF_MarkedGarbage);
        Obj->OnMarkedGarbage();
        
        FScopeLock Lock(DeletesLock);
        PendingDeletes.push(Obj);
    }

    void CollectGarbage()
    {
        LUMINA_PROFILE_SCOPE();
        while (!PendingDeletes.empty())
        {
            CObjectBase* Obj = PendingDeletes.front();
            PendingDeletes.pop();

            Assert(Obj->IsMarkedGarbage())

            Obj->OnDestroy();
            GCObjectAllocator.FreeCObject(Obj);
        }
    }
}
