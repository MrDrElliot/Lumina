#include "Application.h"
#include "ImGui/ImGuiLayer.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Buffer.h"
#include "Source/Runtime/Renderer/Image.h"
#include "Source/Runtime/Renderer/PipelineLibrary.h"
#include "Source/Runtime/Renderer/Renderer.h"
#include "Source/Runtime/Renderer/ShaderLibrary.h"
#include "Source/Runtime/Scene/Scene.h"
#include "Windows/Window.h"


namespace Lumina
{
    FApplication* FApplication::Instance = nullptr;
    
    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        Instance = this;
        
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

                ActiveScene->OnUpdate(1.0f);
                Window->OnUpdate(1.0f);
                
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
        
    }

    void FApplication::OnShutdown()
    {
        bRunning = false;

        LE_LOG_WARN("Lumina Engine: Shutting Down");
        
        FRenderer::Shutdown();
        Window->Shutdown();
        glfwTerminate();
        

        FLog::Shutdown();
    }
    

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        Window = FWindow::Create(InSpecs);
        Window->Init();
        Window->SetEventCallback(HZ_BIND_EVENT_FN(OnEvent));
        
    }

    void FApplication::PreFrame()
    {
        FRenderer::BeginFrame();
    }

    void FApplication::PostFrame()
    {
        
        FRenderer::Render();
        FRenderer::EndFrame();
        glfwPollEvents();
        
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

    void FApplication::InitImGuiLayer()
    {
        ImGuiLayer = FImGuiLayer::Create();
        PushOverlay(ImGuiLayer);
    }

    void FApplication::RenderImGui()
    {
        ImGuiLayer->Begin();

        for(auto Layer : LayerStack)
        {
            Layer->OnImGuiRender();
        }

        ImGuiLayer->End();
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
