#include "Application.h"

#include "Source/Runtime/Assets/AssetManager/AssetManager.h"
#include "Source/Runtime/ImGui/ImGuiRenderer.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Buffer.h"
#include "Source/Runtime/Renderer/PipelineLibrary.h"
#include "Source/Runtime/Renderer/Renderer.h"
#include "Source/Runtime/Renderer/ShaderLibrary.h"
#include "Source/Runtime/Scene/Scene.h"
#include "Windows/Window.h"


namespace Lumina
{
    FApplication* FApplication::Instance = nullptr;
    static double accumTime = 0.0;

    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        Instance = this;
        AssetManager = std::make_shared<FAssetManager>(InAppSpecs);
        
    }

    FApplication::~FApplication()
    {
        
    }
     
    void FApplication::Run()
    {
        /* Application Initialization */
        OnInit();
        
        while(!ShouldExit())
        {
            if (!IsMinimized())
            {
                PreFrame();

                ActiveScene->OnUpdate(Stats.DeltaTime);
                Window->OnUpdate(Stats.DeltaTime);
                FImGuiRenderer::Render(Stats.DeltaTime);
                
                PostFrame();
            }
        }

        /* Application Shutdown */
        OnShutdown();
    }

    int FApplication::Close()
    {
        return 0;
    }

    void FApplication::OnInit()
    {
        /* Initialize Logging */
        FLog::Init();

        LE_LOG_INFO("{0} Initializing", AppSpecs.Name);
        
        /* Create application window */
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = AppSpecs.Name;
        AppWindowSpecs.Width = AppSpecs.WindowWidth;
        AppWindowSpecs.Height = AppSpecs.WindowHeight;
        CreateApplicationWindow(AppWindowSpecs);
        
        FRenderConfig RenderConfig;
        RenderConfig.Window = Window.get();
        RenderConfig.FramesInFlight = 2;
        FRenderer::Init(RenderConfig);

        ActiveScene = std::make_shared<LScene>();

        if(ShouldRenderImGui())
        {
            FImGuiRenderer::Init();
        }
        
    }

    void FApplication::OnShutdown()
    {
        bRunning = false;

        LE_LOG_WARN("Lumina Engine: Shutting Down");
        ActiveScene->Shutdown();
        if (ShouldRenderImGui())
        {
          FRenderer::Shutdown();
        }
        FImGuiRenderer::Shutdown();
        Window->Shutdown();
        glfwTerminate();
        

        FLog::Shutdown();
    }
    

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        Window = FWindow::Create(InSpecs);
        Window->Init();
        Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
        
    }

    void FApplication::PreFrame()
    {

        Stats.DeltaTime = (Stats.CurrentFrameTime - Stats.LastFrameTime);
        Stats.LastFrameTime = Stats.CurrentFrameTime;

        Stats.FPS = static_cast<int>(Stats.DeltaTime / 60.0f);
        accumTime += Stats.DeltaTime;
        
        if (accumTime >= 1.0)
        {
            Stats.FPS = Stats.FrameCount;
            Stats.FrameCount = 0;
            accumTime -= 1.0;
        }

        
        FRenderer::BeginFrame();
        if(ShouldRenderImGui())
        {
            FImGuiRenderer::BeginFrame();
        }
    }

    void FApplication::PostFrame()
    {
        if(ShouldRenderImGui())
        {
            FImGuiRenderer::EndFrame();
        }
        FRenderer::Render();
        FRenderer::EndFrame();
        glfwPollEvents();
        
        Stats.CurrentFrameTime = glfwGetTime();
        
    }


    void FApplication::PushLayer(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PushLayer(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PushOverlay(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PushOverlay(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PopLayer(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PopLayer(InLayer);
        InLayer->OnDetach();
    }

    void FApplication::PopOverlay(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PopOverlay(InLayer);
        InLayer->OnDetach();
    }
    
    void FApplication::OnEvent(FEvent& Event)
    {
        for (auto Layer : LayerStack)
        {
            Layer->OnEvent(Event);
        }
    }

    bool FApplication::ShouldExit() const
    {
        return glfwWindowShouldClose(Window->GetWindow()) || !bRunning;
    }
}
