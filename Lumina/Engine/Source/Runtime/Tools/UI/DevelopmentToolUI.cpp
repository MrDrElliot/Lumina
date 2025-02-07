#include "DevelopmentToolUI.h"

#include "ImGui/Vulkan/VulkanImGuiRender.h"


namespace Lumina
{
    void IDevelopmentToolUI::Update(const FUpdateContext& UpdateContext)
    {
        OnUpdate(UpdateContext);
    }

    void IDevelopmentToolUI::StartFrame(const FUpdateContext& UpdateContext)
    {
        OnStartFrame(UpdateContext);
    }

    void IDevelopmentToolUI::EndFrame(const FUpdateContext& UpdateContext)
    {
        OnEndFrame(UpdateContext);
    }
}
