#include "Application.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "ImGui/ImGuiLayer.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanSwapChain.h"
#include "Source/Runtime/Renderer/RenderContext.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanRenderContext.h"
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
                CheckWindowResized();
                RenderImGui();
                Window->OnUpdate(1.0f);
                RenderThread->Dispatch();
                TestScene->OnUpdate(0.0f);
            }
        }

        /* Application Shutdown */
        OnShutdown();
    }

    void FApplication::OnInit()
    {
        /* Initialize Logging */
        FLog::Init();
        
        /* Create Renderer Context */
        RenderContext = FRenderContext::Create();
        
        /* Create application window */
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = AppSpecs.Name;
        AppWindowSpecs.Width = AppSpecs.WindowWidth;
        AppWindowSpecs.Height = AppSpecs.WindowHeight;
        CreateApplicationWindow(AppWindowSpecs);

        TestScene = new LScene();

    }

    void FApplication::OnShutdown()
    {
        Window->Shutdown();
        
        RenderThread->RequestStop();
        RenderThread->Join();
        
    }
    

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        FVulkanSwapChain* NewSwapChain = FVulkanSwapChain::Create();
        Window.reset(FWindow::Create(InSpecs, NewSwapChain));

        /* Window Init must be called before initializing a swap chain or the GLFWwindow will be null */
        Window->Init();
        Window->SetEventCallback(HZ_BIND_EVENT_FN(OnEvent));
        
        NewSwapChain->Init(Window.get());
        
    }

    void FApplication::CheckWindowResized()
    {
        if(RenderContext->Get<FVulkanRenderContext>()->GetActiveSwapChain()->IsResizeRequested())
        {
            int w, h;
            glfwGetWindowSize(Window->GetWindow(), &w, &h);
            RenderContext->Get<FVulkanRenderContext>()->GetActiveSwapChain()->Resize(w, h);
        }
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
