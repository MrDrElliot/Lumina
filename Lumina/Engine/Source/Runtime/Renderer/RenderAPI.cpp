#include "RenderAPI.h"

#include "RHI/Vulkan/VulkanRenderAPI.h"

namespace Lumina
{
    IRenderAPI* IRenderAPI::Create(const FRenderConfig& InConfig)
    {
        return new FVulkanRenderAPI(InConfig);
    }
}
