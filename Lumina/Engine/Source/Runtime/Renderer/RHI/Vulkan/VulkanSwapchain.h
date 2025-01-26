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
        void CreateImages() override;
        void DestroySurface() override;
        void DestroySwapchain() override;
        void RecreateSwapchain() override;

        bool BeginFrame() override;
        void EndFrame() override;

        uint32 GetCurrentFrameIndex() override { return CurrentFrameIndex; }
        uint32 GetCurrentImageIndex() override { return CurrentImageIndex;  }

        VkSemaphore             GetAquireSemaphore() { return AquireSemaphores[CurrentFrameIndex]; }
        VkSemaphore             GetPresentSemaphore() { return PresentSemaphores[CurrentImageIndex]; }

        VkFence                 GetCurrentFence() { return Fences[CurrentFrameIndex]; }
        TRefPtr<FVulkanImage>&  GetCurrentImage() { return Images[CurrentImageIndex]; }
        VkSwapchainKHR          GetSwapchain() { return Swapchain; }
        VkSurfaceKHR            GetSurface() { return Surface; }
        VkFormat&               GetFormat() { return Format; }
        
    private:

        VkSurfaceKHR    Surface = VK_NULL_HANDLE;
        VkFormat        Format;

        VkSwapchainKHR      Swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR  SurfaceFormat;
        VkPresentModeKHR    CurrentPresentMode;
        
        TArray<TRefPtr<FVulkanImage>>  Images;
        TArray<VkSemaphore>            PresentSemaphores;
        TArray<VkSemaphore>            AquireSemaphores;
        TArray<VkFence>                Fences;
        
        uint32 CurrentFrameIndex = 0;
        uint32 CurrentImageIndex = 0;
    };
}
