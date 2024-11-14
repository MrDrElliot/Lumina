#pragma once
#include "ImGuiWindow.h"

namespace Lumina
{
    class AssetDebugWindow : public EditorImGuiWindow
    {
    public:
    
        AssetDebugWindow()
        {
            Name = "Asset Debug";
            bVisible = false;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
    
    };
}
