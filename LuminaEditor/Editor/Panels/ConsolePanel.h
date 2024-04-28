#pragma once
#include "EditorPanel.h"


namespace Lumina
{
    class FConsolePanel : public FEditorPanel
    {
    public:
        
        FConsolePanel();
        ~FConsolePanel();

        void OnAdded() override;
        void OnEvent(FEvent& Event) override;
        void OnRemoved() override;
        void OnRender(double DeltaTime) override;
        void OnNewScene() override;
        
    };
}
