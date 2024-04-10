#pragma once
#include "EditorPanel.h"
#include "imgui.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanTypes.h"

namespace Lumina
{
    class FSceneViewPanel : public FEditorPanel
    {
    public:

        FSceneViewPanel();
        ~FSceneViewPanel();

        void OnNewScene() override;
        void OnAdded() override;
        void OnRemoved() override;
        
        void OnRender() override;
        void OnImGui() override;

    private:

        ImVec2 Region;
        
    };
}
