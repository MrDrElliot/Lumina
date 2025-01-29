#pragma once
#include "ImGuiWindow.h"

namespace Lumina
{
    class FEntityPropertyPanel : public EditorImGuiWindow
    {
    public:

        FEntityPropertyPanel()
        {
            Name = "Properties";
            bVisible = true;
            bShowInWindows = true;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;


    private:
    
    };
}
