#include "Swapchain.h"

#include "RHI/Vulkan/VulkanSwapchain.h"

namespace Lumina
{
    std::shared_ptr<FSwapchain> FSwapchain::Create(const FSwapchainSpec& InSpec)
    {
        return std::make_shared<FVulkanSwapchain>(InSpec);
    }
}
