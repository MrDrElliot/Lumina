#pragma once

#include "SceneTypes.h"
#include "Memory/RefCounted.h"
#include "Subsystems/Subsystem.h"


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
        
        TRefPtr<FScene> CreateScene(ESceneType InType);
        void DestroyScene(const TRefPtr<FScene>& SceneToRemove);

        void GetAllScenes(TVector<TRefPtr<FScene>>& OutScenes);
        TRefPtr<FScene> GetGameScene();
    
    private:

        TVector<TRefPtr<FScene>>    Scenes;
        TRefPtr<FScene>             GameScene;
        
    };
}
