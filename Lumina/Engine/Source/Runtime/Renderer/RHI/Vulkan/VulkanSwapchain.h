#pragma once

#include <optional>

#include "Source/Runtime/Renderer/Swapchain.h"
#include <vulkan/vulkan_core.h>

namespace Lumina
{
    class FVulkanSwapchain : public FSwapchain
    {
    public:

        FVulkanSwapchain(const FSwapchainSpec& InSpec);
        

        void CreateSurface(const FSwapchainSpec& InSpec) override;
        void CreateSwapchain(const FSwapchainSpec& InSpec) override;

        void DestroySurface() override;
        void DestroySwapchain() override;

        void BeginFrame() override;
        void EndFrame() override;

        uint32_t GetCurrentFrameIndex() { return CurrentFrameIndex; }
        FSwapchainSemaphores GetSemaphores() { return Semaphores[CurrentFrameIndex]; }
        VkFence GetCurrentFence() { return Fences[CurrentFrameIndex]; }
        std::shared_ptr<FVulkanImage> GetCurrentImage() { return Images[CurrentImageIndex]; }
        VkSwapchainKHR GetSwapchain() { return Swapchain.value(); }
        VkSurfaceKHR GetSurface() { return Surface; }
        VkFormat& GetFormat() { return Format; }
        
    private:

        VkSurfaceKHR Surface;
        VkFormat Format;

        std::optional<VkSwapchainKHR> Swapchain;
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR CurrentPresentMode;
        
        std::vector<std::shared_ptr<FVulkanImage>> Images;
        std::vector<FSwapchainSemaphores> Semaphores;
        std::vector<VkFence> Fences;

        uint32_t CurrentFrameIndex;
        uint32_t CurrentImageIndex = 0;
    };
}
