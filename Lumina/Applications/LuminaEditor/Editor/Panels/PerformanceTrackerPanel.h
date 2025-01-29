#pragma once
#include "ImGuiWindow.h"

namespace Lumina
{
    class FPerformanceTrackerPanel : public EditorImGuiWindow
    {
    public:

        FPerformanceTrackerPanel()
        {
            Name = "Performance Metrics";
            bVisible = false;
            bShowInWindows = true;
        }
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
    
    };
}
