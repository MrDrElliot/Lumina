#include "SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene.h"
#include "Renderer/PrimitiveDrawManager.h"

namespace Lumina
{
    void FSceneManager::Initialize(const FSubsystemManager& Manager)
    {
    }

    void FSceneManager::Deinitialize()
    {
        for (FManagedScene& Scene : Scenes)
        {
            Scene.Scene->Shutdown();
            Scene.SceneRenderer->Deinitialize();

            FMemory::Delete(Scene.Scene);
            FMemory::Delete(Scene.SceneRenderer);
        }
        
        Scenes.clear();
    }

    void FSceneManager::StartFrame()
    {
        for (FManagedScene& Scene : Scenes)
        {
            Scene.SceneRenderer->StartScene(Scene.Scene);
            Scene.Scene->GetPrimitiveDrawManager()->StartDraw();
            Scene.Scene->StartFrame();       
        }
    }

    void FSceneManager::UpdateScenes(const FUpdateContext& UpdateContext)
    {
        for (const FManagedScene& Scene : Scenes)
        {
            Scene.Scene->Update(UpdateContext);       
        }
    }
    

    void FSceneManager::EndFrame()
    {
        for (const FManagedScene& Scene : Scenes)
        {
            Scene.Scene->EndFrame();
            Scene.Scene->GetPrimitiveDrawManager()->EndDraw();
            Scene.SceneRenderer->EndScene(Scene.Scene);
        }
    }

    FScene* FSceneManager::CreateScene(ESceneType InType)
    {
        if (InType == ESceneType::Game)
        {
            AssertMsg(GameScene == nullptr, "Only one game scene is allowed"); 
        }

        FScene* NewScene = FMemory::New<FScene>(InType);
        FSceneRenderer* SceneRenderer = FMemory::New<FSceneRenderer>();
        SceneRenderer->Initialize();

        Scenes.emplace_back(NewScene, SceneRenderer);
        return NewScene;
    }

    void FSceneManager::DestroyScene(FScene* SceneToRemove)
    {
        Assert(SceneToRemove != nullptr);
        
        auto Itr = eastl::find_if(Scenes.begin(), Scenes.end(), [SceneToRemove] (const FManagedScene& Scene) 
        {
            return Scene.Scene == SceneToRemove;
        });
        AssertMsg(Itr != Scenes.end(), "Scene was not found in manager!");

        FScene* Scene = Itr->Scene;
        FSceneRenderer* SceneRenderer = Itr->SceneRenderer;
        Scene->Shutdown();
        SceneRenderer->Deinitialize();        
        
        if (SceneToRemove == GameScene)
        {
            GameScene = nullptr;
        }

        
        FMemory::Delete(Scene);
        FMemory::Delete(SceneRenderer);
        
        Scenes.erase(Itr);
    }

    FSceneRenderer* FSceneManager::GetSceneRendererForScene(const FScene* Scene)
    {
        auto Itr = eastl::find_if(Scenes.begin(), Scenes.end(), [Scene] (const FManagedScene& ItrScene) 
        {
            return ItrScene.Scene == Scene;
        });
        
        return Itr != Scenes.end() ? Itr->SceneRenderer : nullptr;
    }
}
