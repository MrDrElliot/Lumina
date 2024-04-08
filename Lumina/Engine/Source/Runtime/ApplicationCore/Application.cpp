#include "Application.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "ImGui/ImGuiLayer.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanSwapChain.h"
#include "Source/Runtime/Renderer/RenderContext.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanRenderContext.h"
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
        
        while(!ShouldExit() || !glfwWindowShouldClose(Window->GetWindow()))
        {
            if (!IsMinimized())
            {
                CheckWindowResized();
                RenderImGui();
                Window->OnUpdate(1.0f);
                
                RenderThread->Dispatch();
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
        
        NewSwapChain->Init(Window.get());
        
    }

    void FApplication::CheckWindowResized()
    {
        if(RenderContext->Get<FVulkanRenderContext>()->GetActiveSwapChain()->IsResizeRequested())
        {
            RenderContext->Get<FVulkanRenderContext>()->GetActiveSwapChain()->Resize();
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
}
