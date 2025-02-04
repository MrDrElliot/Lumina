#pragma once

#include "imgui.h"
#include "ImGuiWindow.h"

namespace Lumina
{
    class FScene;
    class ViewportLayer : public EditorImGuiWindow
    {
    public:
    
        ViewportLayer(const eastl::weak_ptr<FScene>& InScene)
        {
            Scene = InScene;
            Name = "Viewport";
            bVisible = true;
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;

    private:

        ImVec2 Region;
        eastl::weak_ptr<FScene> Scene;
    
    };
}
