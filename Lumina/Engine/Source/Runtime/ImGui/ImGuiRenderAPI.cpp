#include "ImGuiRenderAPI.h"

#include <memory>

#include "Vulkan/VulkanImGuiRenderAPI.h"


namespace Lumina
{
    TSharedPtr<FImGuiRenderAPI> FImGuiRenderAPI::Create()
    {
        return MakeSharedPtr<FVulkanImGuiRenderAPI>();
    }
    
}
