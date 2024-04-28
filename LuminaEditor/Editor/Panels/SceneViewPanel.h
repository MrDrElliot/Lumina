#pragma once
#include <memory>

#include "EditorPanel.h"
#include "imgui.h"



namespace Lumina
{
    class LScene;
    
    class FSceneViewPanel : public FEditorPanel
    {
    public:

        FSceneViewPanel(std::shared_ptr<LScene> InScene);
        ~FSceneViewPanel() override;

        void OnEvent(FEvent& Event) override;
        void OnNewScene() override;
        void OnAdded() override;
        void OnRemoved() override;
        
        void OnRender(double DeltaTime) override;

    private:

        ImVec2 Region;
        std::shared_ptr<LScene> Scene;
        
    };
}
