#include "RenderAPI.h"
#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanRenderAPI.h"

namespace Lumina
{
    IRenderAPI* IRenderAPI::Create()
    {
        return new FVulkanRenderAPI();
    }
}
