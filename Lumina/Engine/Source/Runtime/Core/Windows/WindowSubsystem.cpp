#include "pch.h"
#include "WindowSubsystem.h"
#include "Window.h"

namespace Lumina
{
    void WindowSubsystem::Initialize()
    {
    }

    void WindowSubsystem::Update(double DeltaTime)
    {
        if(Window)
        {
            Window->OnUpdate(DeltaTime);
        }
    }

    void WindowSubsystem::Deinitialize()
    {
        Window->Shutdown();
        Window = nullptr;
    }

    void WindowSubsystem::InitializeWindow(const FWindowSpecs& Specs)
    {
        Window = FWindow::Create(Specs);
        Window->Init();
    }
}
