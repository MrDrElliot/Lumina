#include "RenderAPI.h"

#include "RHI/Vulkan/VulkanRenderAPI.h"

namespace Lumina
{
    FRenderAPI* FRenderAPI::Create(const FRenderConfig& InConfig)
    {
        return new FVulkanRenderAPI(InConfig);
    }
}
