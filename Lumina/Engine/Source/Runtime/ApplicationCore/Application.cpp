#include "Application.h"

#include "backends/imgui_impl_vulkan.h"
#include "ImGui/ImGuiLayer.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/RenderContext.h"
#include "Source/Runtime/Renderer/Renderer.h"
#include "Source/Runtime/Scene/Scene.h"
#include "Windows/Window.h"


namespace Lumina
{
    FApplication* FApplication::Instance = nullptr;
    
    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        Instance = this;

        /* Initialize Render Thread */
        RenderThread = std::make_unique<FRenderThread>();
        RenderThread->Start();
        
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
                
                Window->OnUpdate(1.0f);
                
                PostFrame();
            }
        }

        /* Application Shutdown */
        OnShutdown();
    }

    void FApplication::OnInit()
    {
        /* Initialize Logging */
        FLog::Init();

        
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
    }

    void FApplication::OnShutdown()
    {
        LE_LOG_WARN("Lumina Engine: Shutting Down");
        
        FRenderer::Shutdown();
        Window->Shutdown();
        
        RenderThread->RequestStop();
        RenderThread->Join();
        

        FLog::Shutdown();
    }
    

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        Window.reset(FWindow::Create(InSpecs));
        Window->Init();
        Window->SetEventCallback(HZ_BIND_EVENT_FN(OnEvent));
        
    }

    void FApplication::PreFrame()
    {
        FRenderer::BeginFrame();
    }

    void FApplication::PostFrame()
    {
        FRenderer::EndRender();
        FRenderer::Render();
        FRenderer::EndFrame();
        glfwPollEvents();
    }


    void FApplication::PushLayer(FLayer* InLayer)
    {
        LayerStack.PushLayer(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PushOverlay(FLayer* InLayer)
    {
        LayerStack.PushOverlay(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PopLayer(FLayer* InLayer)
    {
        LayerStack.PopLayer(InLayer);
        InLayer->OnDetach();
    }

    void FApplication::PopOverlay(FLayer* InLayer)
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
