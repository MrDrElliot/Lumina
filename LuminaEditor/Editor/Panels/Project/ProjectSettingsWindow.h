#pragma once
#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class ProjectSettingsWindow : public EditorImGuiWindow
    {
    public:

        ProjectSettingsWindow()
        {
            Name = "Project Settings";
            bVisible = false;
            bShowInWindows = false;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
    
    };
}
