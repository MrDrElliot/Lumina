#include "GarbageCollector.h"

#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"
#include "Core/Object/ObjectBase.h"
#include "Core/Threading/Thread.h"


namespace Lumina::GarbageCollection
{
    static TQueue<CObjectBase*> PendingDeletes;
    static FMutex DeletesLock;
    
    void AddGarbage(CObjectBase* Obj)
    {
        FScopeLock Lock(DeletesLock);

        if (Obj->IsMarkedGarbage())
        {
            LOG_ERROR("[GarbageCollection] Object \"{}\" has already been marked as garbage.", Obj->GetName());
            return;
        }
        
        PendingDeletes.push(Obj);
        
        Obj->SetFlag(OF_MarkedGarbage);
        Obj->OnMarkedGarbage();
    }

    void CollectGarbage()
    {
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
