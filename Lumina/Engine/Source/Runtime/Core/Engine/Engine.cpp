#include "Engine.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Windows/Window.h"
#include "glfw/glfw3.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Application/Application.h"
#include "Core/Object/GarbageCollection/GarbageCollector.h"
#include "Core/Profiler/Profile.h"
#include "Input/InputSubsystem.h"
#include "Renderer/RenderContext.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RenderResource.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/UI/DevelopmentToolUI.h"

namespace Lumina
{
    LUMINA_API FEngine* GEngine = nullptr;


    bool FEngine::Initialize(FApplication* App)
    {
        //-------------------------------------------------------------------------
        // Initialize core engine state.
        //-------------------------------------------------------------------------
        LUMINA_PROFILE_SCOPE();
        
        GEngine = this;
        Application = App;
        
        
        FTaskSystem::Get()->Initialize();
        
        RenderManager = EngineSubsystems.AddSubsystem<FRenderManager>();
        EngineViewport = RenderManager->GetRenderContext()->CreateViewport(Windowing::GetPrimaryWindowHandle()->GetExtent());
        
        InputSubsystem = EngineSubsystems.AddSubsystem<FInputSubsystem>();
        AssetRegistry = EngineSubsystems.AddSubsystem<FAssetRegistry>();
        AssetManager = EngineSubsystems.AddSubsystem<FAssetManager>();
        SceneManager = EngineSubsystems.AddSubsystem<FSceneManager>();
        
        UpdateContext.SubsystemManager = &EngineSubsystems;

        #if WITH_DEVELOPMENT_TOOLS
        CreateDevelopmentTools();
        DeveloperToolUI->Initialize(UpdateContext);
        #endif
        
        PostInitialize();
        
        return true;
    }

    bool FEngine::Shutdown()
    {
        LUMINA_PROFILE_SCOPE();

        //-------------------------------------------------------------------------
        // Shutdown core engine state.
        //-------------------------------------------------------------------------
        
        PreShutdown();
        

        #if WITH_DEVELOPMENT_TOOLS
        DeveloperToolUI->Deinitialize(UpdateContext);
        delete DeveloperToolUI;
        #endif

        EngineSubsystems.RemoveSubsystem<FInputSubsystem>();
        EngineSubsystems.RemoveSubsystem<FAssetRegistry>();
        EngineSubsystems.RemoveSubsystem<FAssetManager>();
        EngineSubsystems.RemoveSubsystem<FSceneManager>();

        EngineViewport.SafeRelease();
        EngineSubsystems.RemoveSubsystem<FRenderManager>();
        

        FTaskSystem::Get()->Shutdown();
        
        return false;
    }

    bool FEngine::Update()
    {
        LUMINA_PROFILE_SCOPE();

        //-------------------------------------------------------------------------
        // Update core engine state.
        //-------------------------------------------------------------------------
        
        UpdateContext.MarkFrameStart();
        RenderManager->FrameStart(UpdateContext);

        bool bRunEngineUpdate = true;

        AssetManager->Update();

        
        if (bRunEngineUpdate)
        {
            UpdateContext.UpdateStage = EUpdateStage::FrameStart;

            // Frame Start
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("FrameStart");
                UpdateCallback(UpdateContext);
                
                SceneManager->StartFrame(UpdateContext);

                InputSubsystem->Update(UpdateContext);
                
                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->StartFrame(UpdateContext);
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                SceneManager->UpdateScenes(UpdateContext);
            }

            // Pre Physics
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Pre-Physics");
                UpdateContext.UpdateStage = EUpdateStage::PrePhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                SceneManager->UpdateScenes(UpdateContext);
            }

            // During Physics
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("During Physics");
                UpdateContext.UpdateStage = EUpdateStage::DuringPhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                SceneManager->UpdateScenes(UpdateContext);
            }

            // Post Physics
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Post Physics");
                UpdateContext.UpdateStage = EUpdateStage::PostPhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                SceneManager->UpdateScenes(UpdateContext);
            }

            // Paused
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Paused");
                UpdateContext.UpdateStage = EUpdateStage::Paused;

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                SceneManager->UpdateScenes(UpdateContext);
            }

            // Frame End
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Frame End");
                UpdateContext.UpdateStage = EUpdateStage::FrameEnd;

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                SceneManager->UpdateScenes(UpdateContext);

                
                SceneManager->EndFrame(UpdateContext);

                
                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->EndFrame(UpdateContext);
                #endif
                
                RenderManager->FrameEnd(UpdateContext);
                
            }
        }
        
        GarbageCollection::CollectGarbage();
        UpdateContext.MarkFrameEnd(glfwGetTime());
        
        return true;
    }

    void FEngine::DrawDevelopmentTools()
    {
        if (Application->HasAnyFlags(EApplicationFlags::DevelopmentTools))
        {
            Application->RenderDeveloperTools(UpdateContext);
        }
    }

    void FEngine::SetEngineViewportSize(const FIntVector2D& InSize)
    {
        EngineViewport->SetSize(InSize);
    }
}
