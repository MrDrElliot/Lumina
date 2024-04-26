#pragma once
#include "EditorPanel.h"

namespace Lumina
{
    class FSceneHierarchyPanel : public FEditorPanel
    {
    public:

        FSceneHierarchyPanel();
        ~FSceneHierarchyPanel();

        void OnAdded() override;
        void OnRemoved() override;
        void OnRender(double DeltaTime) override;
        void OnNewScene() override;
    
    };
}
