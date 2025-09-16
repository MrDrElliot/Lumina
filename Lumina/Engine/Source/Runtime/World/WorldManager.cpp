#include "WorldManager.h"

#include <fastgltf/types.hpp>

#include "World/SceneRenderer.h"
#include "Core/Profiler/Profile.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{
    void FWorldManager::Initialize()
    {
    }

    void FWorldManager::Deinitialize()
    {
        for (FManagedWorld& World : Worlds)
        {
            World.World->ShutdownWorld();
            World.World->MarkGarbage();
        }
        
        Worlds.clear();
    }

    void FWorldManager::UpdateWorlds(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        for (FManagedWorld& World : Worlds)
        {
            if (World.World->IsSuspended())
            {
                continue;
            }

            if (UpdateContext.GetUpdateStage() == EUpdateStage::Paused && World.World->IsPaused())
            {
                World.World->Paused(UpdateContext);
                continue;
            }
            else if (!World.World->IsPaused())
            {
                World.World->Update(UpdateContext);
            }
        }
    }

    void FWorldManager::RenderWorlds(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        FRenderGraph RenderGraph;
        
        for (FManagedWorld& World : Worlds)
        {
            if (World.World->IsSuspended())
            {
                continue;
            }
            
            World.World->Render(RenderGraph);
        }
        
        RenderGraph.Execute();

    }

    void FWorldManager::RemoveWorld(CWorld* World)
    {
        if (Worlds.empty())
        {
            return;
        }

        size_t idx  = World->WorldIndex;
        size_t last = Worlds.size() - 1;

        if (idx != last)
        {
            eastl::swap(Worlds[idx], Worlds[last]);
            Worlds[idx].World->WorldIndex = idx;
        }

        Worlds.pop_back();
    }

    
    void FWorldManager::AddWorld(CWorld* World)
    {
        FManagedWorld MWorld;
        MWorld.World = World;
        World->WorldIndex = Worlds.size(); 
        Worlds.push_back(MWorld);
    }
}
