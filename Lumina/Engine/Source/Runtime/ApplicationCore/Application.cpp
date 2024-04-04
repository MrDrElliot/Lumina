#include "Application.h"

#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    FApplication* FApplication::Instance = nullptr;

    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        
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
                Window->OnUpdate(1.0f);
            }

        }

        /* Application Shutdown */
        OnShutdown();
    }

    void FApplication::Close()
    {

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
    }

    void FApplication::OnShutdown()
    {

    }

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        Window = std::unique_ptr<FWindow>(FWindow::Create(InSpecs));
        Window->Init();
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
