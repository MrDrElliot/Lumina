#include "SystemContext.h"

#include "World/World.h"

namespace Lumina
{
    FSystemContext::FSystemContext(CWorld* World)
    {
        EntityWorld = &World->EntityWorld;
        EntityWorld->Lock();
        DeltaTime = World->GetWorldDeltaTime();
    }

    FSystemContext::~FSystemContext()
    {
        EntityWorld->Unlock();
    }
}
