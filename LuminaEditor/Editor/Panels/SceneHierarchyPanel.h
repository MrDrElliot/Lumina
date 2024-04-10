#pragma once
#include "EditorPanel.h"

namespace Lumina
{
    class FSceneHierarchyPanel : public FEditorPanel
    {
    public:

        FSceneHierarchyPanel();
        ~FSceneHierarchyPanel();

        void OnImGui() override;
        void OnAdded() override;
        void OnRemoved() override;
        void OnRender() override;
        void OnNewScene() override;
    
    };
}
