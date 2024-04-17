#include "RenderContext.h"

#include "RHI/Vulkan/VulkanRenderContext.h"


namespace Lumina
{
    FRenderContext* FRenderContext::Create(const FRenderConfig& InConfig)
    {
        return new FVulkanRenderContext(InConfig);
    }
    
}
