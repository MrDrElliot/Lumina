#include "RendererContext.h"

#include "Vulkan/VulkanRendererContext.h"

namespace Lumina
{
    std::shared_ptr<FRendererContext> FRendererContext::Create()
    {
        return std::make_shared<FVulkanRendererContext>();
    }
}