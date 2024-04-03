#include "Application.h"

#include "Source/Runtime/Log/Log.h"
#include <iostream>

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

        Window = std::unique_ptr<FWindow>(FWindow::Create(AppWindowSpecs));

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

    }

    void FApplication::PushOverlay(FLayer* InLayer)
    {
    }

    void FApplication::PopLayer(FLayer* InLayer)
    {

    }

    void FApplication::PopOverlay(FLayer* InLayer)
    {

    }
}
