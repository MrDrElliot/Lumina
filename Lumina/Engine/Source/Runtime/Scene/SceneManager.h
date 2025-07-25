﻿#pragma once

#include "Scene.h"
#include "SceneTypes.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FSceneRenderer;
}

namespace Lumina
{
    class FScene;
    
    class LUMINA_API FSceneManager : public ISubsystem
    {
    public:

        struct FManagedScene
        {
            FScene*             Scene = nullptr;
            FSceneRenderer*     SceneRenderer = nullptr;

            FORCEINLINE bool operator == (const FManagedScene& Other)
            {
                return Scene == Other.Scene && SceneRenderer == Other.SceneRenderer;
            }
        };

        void Initialize(FSubsystemManager& Manager) override;
        void Deinitialize() override;

        void StartFrame(const FUpdateContext& UpdateContext);
        void UpdateScenes(const FUpdateContext& UpdateContext);
        void EndFrame(const FUpdateContext& UpdateContext);
        
        FScene* CreateScene(ESceneType InType);
        void DestroyScene(FScene* SceneToRemove);

        FORCEINLINE FScene* GetGameScene() const { return GameScene; }

        FSceneRenderer* GetSceneRendererForScene(const FScene* Scene);
    
    private:

        TVector<FManagedScene>      Scenes;
        
        FScene*                     GameScene = nullptr;
        
    };
}
