#include "Swapchain.h"
#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanSwapchain.h"

namespace Lumina
{
    FRHISwapchain FSwapchain::Create(const FSwapchainSpec& InSpec)
    {
        return FRHISwapchain(MakeRefCount<FVulkanSwapchain>(InSpec));
    }
}
