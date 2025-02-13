#pragma once

#include "SceneTypes.h"
#include "Core/UpdateContext.h"
#include "Memory/RefCounted.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FSceneRenderer;
}

namespace Lumina
{
    class FScene;
    
    class FSceneManager : public ISubsystem
    {
    public:

        void Initialize(const FSubsystemManager& Manager) override;
        void Deinitialize() override;

        void StartFrame();
        void UpdateScenes(const FUpdateContext& UpdateContext);
        void EndFrame();
        void RenderScenes(const FUpdateContext& UpdateContext);
        
        FScene* CreateScene(ESceneType InType);
        void DestroyScene(FScene* SceneToRemove);

        void GetAllScenes(TVector<FScene*>& OutScenes) const;
        FORCEINLINE FScene* GetGameScene() const { return GameScene; }
    
    private:

        TVector<FScene*>            Scenes;
        FScene*                     GameScene = nullptr;
        
    };
}
