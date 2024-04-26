#include "ImGuiRenderAPI.h"

#include <memory>

#include "Source/Runtime/Platform/Vulkan/VulkanImGuiRenderAPI.h"

namespace Lumina
{
    std::shared_ptr<FImGuiRenderAPI> FImGuiRenderAPI::Create()
    {
        return std::make_shared<FVulkanImGuiRenderAPI>();
    }
    
}
