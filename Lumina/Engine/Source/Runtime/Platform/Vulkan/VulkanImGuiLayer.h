#pragma once
#include "Source/Runtime/ApplicationCore/ImGui/ImGuiLayer.h"

namespace Lumina
{
    class FVulkanImGuiLayer : public FImGuiLayer
    {
    public:

        void Begin() override;
        void End() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override;

    private:

        float Time = 0.0f;
    
    };
}
