#include "Engine.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Windows/Window.h"
#include "glfw/glfw3.h"
#include "Input/InputSubsystem.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Renderer/RenderManager.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/UI/DevelopmentToolUI.h"

#include "Tools/UI/ImGui/Vulkan/VulkanImGuiRender.h"

namespace Lumina
{
    bool FEngine::Initialize(FApplication* App)
    {
        //-------------------------------------------------------------------------
        // Initialize core engine state.
        //-------------------------------------------------------------------------
        
        GEngine = this;
        Application = App;

        EngineViewport.SetSize(App->GetMainWindow()->GetExtent());
        
        FTaskSystem::Get()->Initialize();
        
        RenderManager = EngineSubsystems.AddSubsystem<FRenderManager>();
        InputSubsystem = EngineSubsystems.AddSubsystem<FInputSubsystem>();
        AssetManagerSubystem = EngineSubsystems.AddSubsystem<FAssetManager>();
        SceneManager = EngineSubsystems.AddSubsystem<FSceneManager>();
        
        #if WITH_DEVELOPMENT_TOOLS
        ImGuiRenderer = EngineSubsystems.AddSubsystem<FVulkanImGuiRender>();
        #endif
        
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
        //-------------------------------------------------------------------------
        // Shutdown core engine state.
        //-------------------------------------------------------------------------
        
        PreShutdown();
        

        #if WITH_DEVELOPMENT_TOOLS
        DeveloperToolUI->Deinitialize(UpdateContext);
        delete DeveloperToolUI;
        EngineSubsystems.RemoveSubsystem<FVulkanImGuiRender>();
        #endif

        EngineSubsystems.RemoveSubsystem<FInputSubsystem>();
        EngineSubsystems.RemoveSubsystem<FAssetManager>();
        EngineSubsystems.RemoveSubsystem<FSceneManager>();
        EngineSubsystems.RemoveSubsystem<FRenderManager>();
        

        FTaskSystem::Get()->Shutdown();
        
        return false;
    }

    bool FEngine::Update()
    {
        //-------------------------------------------------------------------------
        // Update core engine state.
        //-------------------------------------------------------------------------
        
        UpdateContext.MarkFrameStart();
        RenderManager->FrameStart(UpdateContext);

        bool bRunEngineUpdate = true;

        AssetManagerSubystem->Update();

        
        if (bRunEngineUpdate)
        {
            UpdateContext.UpdateStage = EUpdateStage::FrameStart;

            // Frame Start
            //-------------------------------------------------------------------
            {
                UpdateCallback();
                
                SceneManager->StartFrame();

                InputSubsystem->Update(UpdateContext);
                
                #if WITH_DEVELOPMENT_TOOLS
                ImGuiRenderer->StartFrame(RenderManager);
                DeveloperToolUI->StartFrame(UpdateContext);
                #endif

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                SceneManager->UpdateScenes(UpdateContext);
            }

            // Pre Physics
            //-------------------------------------------------------------------
            {
                UpdateContext.UpdateStage = EUpdateStage::PrePhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                SceneManager->UpdateScenes(UpdateContext);
            }

            // During Physics
            //-------------------------------------------------------------------
            {
                UpdateContext.UpdateStage = EUpdateStage::DuringPhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                SceneManager->UpdateScenes(UpdateContext);
            }

            // Post Phsyics
            //-------------------------------------------------------------------
            {
                UpdateContext.UpdateStage = EUpdateStage::PostPhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                SceneManager->UpdateScenes(UpdateContext);
            }

            // Paused
            //-------------------------------------------------------------------
            {
                UpdateContext.UpdateStage = EUpdateStage::Paused;

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                SceneManager->UpdateScenes(UpdateContext);
            }

            // Frame End
            //-------------------------------------------------------------------
            {
                UpdateContext.UpdateStage = EUpdateStage::FrameEnd;

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                SceneManager->UpdateScenes(UpdateContext);

                SceneManager->EndFrame();

                
                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->EndFrame(UpdateContext);
                ImGuiRenderer->EndFrame(RenderManager);
                #endif
                
                RenderManager->FrameEnd(UpdateContext);
                
            }
        }
        
        
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
}
