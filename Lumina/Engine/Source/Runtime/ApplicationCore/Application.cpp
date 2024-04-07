#include "Application.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
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
    }

    FApplication::~FApplication()
    {

    }
     
    void FApplication::Run()
    {
        /* Application Initialization */
        OnInit();
        ImGuiInit();
        
        while(!ShouldExit())
        {
            if (!IsMinimized())
            {
                if(FRenderContext::Get<FVulkanRenderContext>()->IsResizeRequested())
                {
                    Window->GetSwapChain()->Resize();
                }
                Window->OnImGuiUpdate(1.0f);
                Window->OnUpdate(1.0f);
            }
        }

        /* Application Shutdown */
        ImGuiShutdown();
        OnShutdown();
    }

    void FApplication::Close()
    {

    }

    void FApplication::ImGuiInit()
    {

    }

    void FApplication::ImGuiShutdown()
    {
        
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
        
    }

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        FVulkanSwapChain* NewSwapChain = FVulkanSwapChain::Create();
        Window.reset(FWindow::Create(InSpecs, NewSwapChain));

        /* Window Init must be called before initializing a swap chain or the GLFWwindow will be null */
        Window->Init();
        
        NewSwapChain->Init(Window.get());
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
    
}
