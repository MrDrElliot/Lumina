#include "Application.h"

#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/RHI/RendererContext.h"

namespace Lumina
{

    FApplication* FApplication::Instance = nullptr;

    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;

        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = InAppSpecs.Name;
        AppWindowSpecs.Width = InAppSpecs.WindowWidth;
        AppWindowSpecs.Height = InAppSpecs.WindowHeight;

        FLog::Init();

        Window = std::unique_ptr<FWindow>(FWindow::Create(AppWindowSpecs, true));


        uint32_t ExtCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &ExtCount, nullptr);

        LE_LOG_ERROR("{0}", ExtCount);

        LRendererContext::Create();

    }

    FApplication::~FApplication()
    {

    }
     
    void FApplication::Run()
    {
        OnInit();
        
        while(!ShouldExit())
        {
            
            if (!IsMinimized())
            {

                Window->OnUpdate(1.0f);
            }

        }

        OnShutdown();
    }

    void FApplication::Close()
    {

    }

    void FApplication::OnInit()
    {

    }

    void FApplication::OnShutdown()
    {

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
