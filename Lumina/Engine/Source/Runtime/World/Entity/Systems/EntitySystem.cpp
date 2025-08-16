#include "EntitySystem.h"

namespace Lumina
{
    CEntitySystemRegistry* CEntitySystemRegistry::Singleton = nullptr;
    
    void CEntitySystemRegistry::RegisterSystem(class CEntitySystem* NewSystem)
    {
        RegisteredSystems.push_back(NewSystem);
    }

    CEntitySystemRegistry& CEntitySystemRegistry::Get()
    {
        if (Singleton == nullptr)
        {
            Singleton = NewObject<CEntitySystemRegistry>(nullptr, "CEntitySystemRegistry_Singleton");
        }
         
        return *Singleton;
    }

    void CEntitySystemRegistry::GetRegisteredSystems(TVector<TObjectHandle<CEntitySystem>>& Systems)
    {
        Systems = RegisteredSystems;
    }

    void CEntitySystem::PostCreateCDO()
    {
        CEntitySystemRegistry::Get().RegisterSystem(this);
    }
}
