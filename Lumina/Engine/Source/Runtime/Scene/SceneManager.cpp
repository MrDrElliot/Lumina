#include "SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene.h"
#include "Core/UpdateContext.h"

namespace Lumina
{
    void FSceneManager::Initialize(const FSubsystemManager& Manager)
    {
        GameScene = CreateScene(ESceneType::Game);
    }

    void FSceneManager::Deinitialize()
    {
        for (const TRefPtr<FScene>& Scene : Scenes)
        {
            Scene->Shutdown();
        }
        
        GameScene.Release();
        Scenes.clear();
    }

    void FSceneManager::StartFrame()
    {
        for (const TRefPtr<FScene>& Scene : Scenes)
        {
            Scene->StartFrame();       
        }
    }

    void FSceneManager::UpdateScenes(const FUpdateContext& UpdateContext)
    {
        for (const TRefPtr<FScene>& Scene : Scenes)
        {
            Scene->Update(UpdateContext);            
        }
    }

    void FSceneManager::EndFrame()
    {
        
    }

    TRefPtr<FScene> FSceneManager::CreateScene(ESceneType InType)
    {
        if (InType == ESceneType::Game)
        {
            AssertMsg(!GameScene.IsValid(), "Only one game scene is allowed"); 
        }

        TRefPtr<FScene> NewScene = MakeRefPtr<FScene>(InType);
        Scenes.push_back(NewScene);

        return NewScene;
    }

    void FSceneManager::DestroyScene(const TRefPtr<FScene>& SceneToRemove)
    {
        if (SceneToRemove == GameScene)
        {
            GameScene.Release();
            GameScene = nullptr;
        }

        auto Itr = eastl::find(Scenes.begin(), Scenes.end(), SceneToRemove);
        AssertMsg(Itr != Scenes.end(), "Scene was not found in manager!");
        Scenes.erase(Itr);
        SceneToRemove->Shutdown();
    }

    void FSceneManager::GetAllScenes(TVector<TRefPtr<FScene>>& OutScenes)
    {
        OutScenes = Scenes;
    }

    TRefPtr<FScene> FSceneManager::GetGameScene()
    {
        return GameScene;
    }
}
