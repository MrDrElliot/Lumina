#include "EntityComponentRegistry.h"

namespace Lumina
{
    FEntityComponentRegistry& FEntityComponentRegistry::Get()
    {
        static FEntityComponentRegistry Registry;
        return Registry;
    }

    void FEntityComponentRegistry::AddDeferred(RegisterFunc fn)
    {
        Registrants.push_back(fn);
    }

    void FEntityComponentRegistry::RegisterAll()
    {
        for (auto It = Registrants.begin(); It != Registrants.end(); ++It)
        {
            auto FnPtr = *It;
            FnPtr();
        }
            
        Clear();
    }

    void FEntityComponentRegistry::Clear()
    {
        Registrants.clear();
    }

    void ECS::AddDeferredComponentRegistry(RegisterFunc Func)
    {
        FEntityComponentRegistry::Get().AddDeferred(Func);
    }
}
