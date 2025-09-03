#pragma once

#include "World.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    enum class EWorldType : uint8;
    class FSceneRenderer;
}

namespace Lumina
{
    class LUMINA_API FWorldManager : public ISubsystem
    {
    public:

        struct FManagedWorld
        {
            EWorldType              Type;
            TObjectHandle<CWorld>   World;
        };

        void Initialize() override;
        void Deinitialize() override;

        void TickWorlds(const FUpdateContext& UpdateContext);

        void RemoveWorld(CWorld* World);
        void AddWorld(CWorld* World);
    
    private:

        TVector<FManagedWorld> Worlds;
        
    };
}
