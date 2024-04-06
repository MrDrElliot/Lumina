#include "RenderContext.h"
#include "Vulkan\VulkanRenderContext.h"
#include "Source/Runtime/ApplicationCore/Application.h"


namespace Lumina
{
    FRenderContext* FRenderContext::Create()
    {
        return new FVulkanRenderContext();
    }
    
}
