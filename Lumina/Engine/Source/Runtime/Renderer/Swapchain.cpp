#include "Swapchain.h"
#include "Image.h"
#include "RHI/Vulkan/VulkanSwapchain.h"

namespace Lumina
{
    TRefPtr<FSwapchain> FSwapchain::Create(const FSwapchainSpec& InSpec)
    {
        return MakeRefPtr<FVulkanSwapchain>(InSpec);
    }
}
