#pragma once
#include "ImGuiWindow.h"


namespace Lumina
{
    class ToolbarWindow : public EditorImGuiWindow
    {
    public:

        ToolbarWindow()
        {
            Name = "Toolbar";
            bVisible = true;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;

    private:

        bool bShowImGuiDemo = false;
        bool bShowProjectSettings = false;
    
    };
}
