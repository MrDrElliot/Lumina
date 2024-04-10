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
        void OnRender() override;
        void OnImGui() override;
        void OnNewScene() override;
    
    };
}
