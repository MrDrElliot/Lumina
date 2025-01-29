#pragma once
#include "ImGuiWindow.h"

namespace Lumina
{
    class ApplicationStats : public EditorImGuiWindow
    {
    public:

        ApplicationStats()
        {
            Name = "Application Stats";
            bVisible = false;
        }
    
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
    };
}
