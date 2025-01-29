#pragma once
#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class SceneSettings : public EditorImGuiWindow
    {
    public:

        SceneSettings()
        {
            Name = "Scene Settings";
            bShowInWindows = true;
            bVisible = true;
        }
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
    };
}