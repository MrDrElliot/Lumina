#pragma once
#include "ImGuiWindow.h"

namespace Lumina
{
    class FEditorSettingsPanel : public EditorImGuiWindow
    {
    public:

        FEditorSettingsPanel()
        {
            Name = "Editor Settings";
            bVisible = false;
            bShowInWindows = false;
        }
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
    
    };
}
