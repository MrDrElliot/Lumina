#pragma once
#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class AScene;

    class SceneOutliner : public EditorImGuiWindow
    {
    public:

        SceneOutliner(const eastl::weak_ptr<AScene>& InScene)
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

        eastl::weak_ptr<AScene> mScene;
    
    };
}
