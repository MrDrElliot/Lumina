#include "SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene.h"
#include "Core/Profiler/Profile.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{
    void FSceneManager::Initialize(FSubsystemManager& Manager)
    {
    }

    void FSceneManager::Deinitialize()
    {
        for (FManagedScene& Scene : Scenes)
        {
            Scene.Scene->Shutdown();
            Scene.SceneRenderer->Deinitialize();

            Memory::Delete(Scene.Scene);
            Memory::Delete(Scene.SceneRenderer);
        }
        
        Scenes.clear();
    }

    void FSceneManager::StartFrame(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        for (FManagedScene& Scene : Scenes)
        {
            if (Scene.Scene->IsPaused())
            {
                continue;
            }
            
            Scene.SceneRenderer->StartScene(UpdateContext);
            Scene.Scene->StartFrame();       
        }
    }

    void FSceneManager::UpdateScenes(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        for (const FManagedScene& Scene : Scenes)
        {
            if (Scene.Scene->IsPaused())
            {
                continue;
            }
            
            Scene.Scene->Update(UpdateContext);       
        }
    }
    

    void FSceneManager::EndFrame(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        for (const FManagedScene& Scene : Scenes)
        {
            if (Scene.Scene->IsPaused())
            {
                continue;
            }
            
            Scene.Scene->EndFrame();
            Scene.SceneRenderer->EndScene(UpdateContext);
        }
    }

    FScene* FSceneManager::CreateScene(ESceneType InType)
    {
        LUMINA_PROFILE_SCOPE();
        if (InType == ESceneType::Game)
        {
            AssertMsg(GameScene == nullptr, "Only one game scene is allowed"); 
        }

        FScene* NewScene = Memory::New<FScene>(InType);
        FSceneRenderer* SceneRenderer = Memory::New<FSceneRenderer>(NewScene);
        SceneRenderer->Initialize();

        Scenes.emplace_back(NewScene, SceneRenderer);
        return NewScene;
    }

    void FSceneManager::DestroyScene(FScene* SceneToRemove)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(SceneToRemove != nullptr)
        
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

        
        Memory::Delete(Scene);
        Memory::Delete(SceneRenderer);
        
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
