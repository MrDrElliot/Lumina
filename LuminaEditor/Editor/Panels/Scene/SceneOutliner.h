#pragma once
#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class LScene;

    class SceneOutliner : public EditorImGuiWindow
    {
    public:

        SceneOutliner(const std::shared_ptr<LScene>& InScene)
        {
            mScene = InScene;
            
            Name = "Scene Outliner";
            bVisible = true;

        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;


    private:

        std::shared_ptr<LScene> mScene;
    
    };
}
