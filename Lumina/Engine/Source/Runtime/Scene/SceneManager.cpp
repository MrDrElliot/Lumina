#include "SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene.h"
#include "Core/UpdateContext.h"

namespace Lumina
{
    void FSceneManager::Initialize(const FSubsystemManager& Manager)
    {
    }

    void FSceneManager::Deinitialize()
    {
        for (FScene* Scene : Scenes)
        {
            Scene->Shutdown();
            FMemory::Delete(Scene);
        }
        
        Scenes.clear();
    }

    void FSceneManager::StartFrame()
    {
        for (FScene* Scene : Scenes)
        {
            Scene->StartFrame();       
        }
    }

    void FSceneManager::UpdateScenes(const FUpdateContext& UpdateContext)
    {
        for (FScene* Scene : Scenes)
        {
            Scene->Update(UpdateContext);       
        }
    }

    void FSceneManager::RenderScenes(const FUpdateContext& UpdateContext)
    {
        FSceneRenderer* SceneRenderer = UpdateContext.GetSubsystem<FSceneRenderer>();
        Assert(SceneRenderer != nullptr);
        
        for (FScene* Scene : Scenes)
        {
            SceneRenderer->RenderScene(Scene);  
        }
    }

    void FSceneManager::EndFrame()
    {
        for (FScene* Scene : Scenes)
        {
            Scene->EndFrame();       
        }
    }

    FScene* FSceneManager::CreateScene(ESceneType InType)
    {
        if (InType == ESceneType::Game)
        {
            AssertMsg(GameScene == nullptr, "Only one game scene is allowed"); 
        }

        FScene* NewScene = FMemory::New<FScene>(InType);
        Scenes.push_back(NewScene);

        return NewScene;
    }

    void FSceneManager::DestroyScene(FScene* SceneToRemove)
    {
        Assert(SceneToRemove != nullptr);
        
        auto Itr = eastl::find(Scenes.begin(), Scenes.end(), SceneToRemove);
        AssertMsg(Itr != Scenes.end(), "Scene was not found in manager!");
        Scenes.erase(Itr);
        SceneToRemove->Shutdown();

        if (SceneToRemove == GameScene)
        {
            GameScene = nullptr;
        }

        FMemory::Delete(SceneToRemove);
        
    }

    void FSceneManager::GetAllScenes(TVector<FScene*>& OutScenes) const
    {
        OutScenes = Scenes;
    }


}
