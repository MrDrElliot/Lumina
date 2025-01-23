#pragma once
#include "EditorLayer.h"
#include "imgui.h"
#include "ImGuiWindow.h"

namespace Lumina
{
    class LScene;
    class ViewportLayer : public EditorImGuiWindow
    {
    public:
    
        ViewportLayer(const std::weak_ptr<Lumina::LScene>& InScene)
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
        std::weak_ptr<LScene> Scene;
    
    };
}
