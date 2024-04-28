#pragma once
#include "EditorPanel.h"

namespace Lumina
{
    class FPropertyDetailsPanel : public FEditorPanel
    {
    public:
        
        FPropertyDetailsPanel();
        ~FPropertyDetailsPanel();

        void OnEvent(FEvent& Event) override;
        void OnAdded() override;
        void OnRemoved() override;
        void OnRender(double DeltaTime) override;
        void OnNewScene() override;
    
    };
}
