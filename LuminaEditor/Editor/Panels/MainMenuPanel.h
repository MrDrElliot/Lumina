#pragma once
#include "EditorPanel.h"

namespace Lumina
{
    class FMainMenuPanel : public FEditorPanel
    {
    public:

        FMainMenuPanel();
        ~FMainMenuPanel();

        void OnAdded() override;
        void OnRemoved() override;
        void OnEvent(FEvent& Event) override;
        void OnRender(double DeltaTime) override;
        void OnNewScene() override;

        bool bShowDemo = false;
    
    };
}
