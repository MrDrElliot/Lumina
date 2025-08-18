#include "Engine.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Windows/Window.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Application/Application.h"
#include "Core/Object/GarbageCollection/GarbageCollector.h"
#include "Core/Profiler/Profile.h"
#include "Input/InputSubsystem.h"
#include "Renderer/RenderContext.h"
#include "World/WorldManager.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RHIGlobals.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/UI/DevelopmentToolUI.h"

namespace Lumina
{
    LUMINA_API FEngine* GEngine;
    
    bool FEngine::Init(FApplication* App)
    {
        //-------------------------------------------------------------------------
        // Initialize core engine state.
        //-------------------------------------------------------------------------
        LUMINA_PROFILE_SCOPE();
        
        GEngine = this;
        Application = App;

        
        FTaskSystem::Get().Initialize();
        
        RenderManager = EngineSubsystems.AddSubsystem<FRenderManager>();
        EngineViewport = GRenderContext->CreateViewport(Windowing::GetPrimaryWindowHandle()->GetExtent());
        
        ProcessNewlyLoadedCObjects();
        
        InputSubsystem = EngineSubsystems.AddSubsystem<FInputSubsystem>();
        AssetRegistry = EngineSubsystems.AddSubsystem<FAssetRegistry>();
        AssetManager = EngineSubsystems.AddSubsystem<FAssetManager>();
        WorldManager = EngineSubsystems.AddSubsystem<FWorldManager>();
        
        UpdateContext.SubsystemManager = &EngineSubsystems;

        #if WITH_DEVELOPMENT_TOOLS
        DeveloperToolUI = CreateDevelopmentTools();
        DeveloperToolUI->Initialize(UpdateContext);
        #endif
        
        return true;
    }

    bool FEngine::Shutdown()
    {
        LUMINA_PROFILE_SCOPE();

        //-------------------------------------------------------------------------
        // Shutdown core engine state.
        //-------------------------------------------------------------------------

        #if WITH_DEVELOPMENT_TOOLS
        DeveloperToolUI->Deinitialize(UpdateContext);
        delete DeveloperToolUI;
        #endif

        EngineSubsystems.RemoveSubsystem<FInputSubsystem>();
        EngineSubsystems.RemoveSubsystem<FAssetRegistry>();
        EngineSubsystems.RemoveSubsystem<FAssetManager>();
        EngineSubsystems.RemoveSubsystem<FWorldManager>();

        ShutdownCObjectSystem();
        
        EngineViewport.SafeRelease();
        
        EngineSubsystems.RemoveSubsystem<FRenderManager>();
        

        FTaskSystem::Get().Shutdown();
        
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
        
        
        if (bRunEngineUpdate)
        {
            UpdateContext.UpdateStage = EUpdateStage::FrameStart;

            // Frame Start
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("FrameStart");
                
                InputSubsystem->Update(UpdateContext);
                
                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->StartFrame(UpdateContext);
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                WorldManager->TickWorlds(UpdateContext);
            }

            // Pre Physics
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Pre-Physics");
                UpdateContext.UpdateStage = EUpdateStage::PrePhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                WorldManager->TickWorlds(UpdateContext);
            }

            // During Physics
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("During Physics");
                UpdateContext.UpdateStage = EUpdateStage::DuringPhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif
                
                WorldManager->TickWorlds(UpdateContext);
            }

            // Post Physics
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Post Physics");
                UpdateContext.UpdateStage = EUpdateStage::PostPhysics;


                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                WorldManager->TickWorlds(UpdateContext);
            }

            // Paused
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Paused");
                UpdateContext.UpdateStage = EUpdateStage::Paused;

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                WorldManager->TickWorlds(UpdateContext);
            }

            // Frame End
            //-------------------------------------------------------------------
            {
                LUMINA_PROFILE_SECTION("Frame End");
                UpdateContext.UpdateStage = EUpdateStage::FrameEnd;

                #if WITH_DEVELOPMENT_TOOLS
                DeveloperToolUI->Update(UpdateContext);
                #endif

                WorldManager->TickWorlds(UpdateContext);
                
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

    #if WITH_DEVELOPMENT_TOOLS
    void FEngine::DrawDevelopmentTools()
    {
        if (Application->HasAnyFlags(EApplicationFlags::DevelopmentTools))
        {
            Application->RenderDeveloperTools(UpdateContext);
        }
    }
    #endif
    
    void FEngine::SetEngineViewportSize(const FIntVector2D& InSize)
    {
        EngineViewport->SetSize(InSize);
    }

}
