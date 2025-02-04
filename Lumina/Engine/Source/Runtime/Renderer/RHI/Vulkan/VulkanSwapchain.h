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
        

        void CreateSurface(IRenderContext* Context, const FSwapchainSpec& InSpec) override;
        void CreateSwapchain(IRenderContext* Context, const FSwapchainSpec& InSpec) override;
        void CreateImages() override;
        void DestroySurface() override;
        void DestroySwapchain() override;
        void RecreateSwapchain() override;

        bool BeginFrame() override;
        void EndFrame() override;

        VkSemaphore             GetAquireSemaphore() { return AquireSemaphores[CurrentFrameIndex]; }
        VkSemaphore             GetPresentSemaphore() { return PresentSemaphores[CurrentImageIndex]; }

        VkFence                 GetCurrentFence() { return Fences[CurrentFrameIndex]; }
        VkSwapchainKHR          GetSwapchain() { return Swapchain; }
        VkSurfaceKHR            GetSurface() { return Surface; }
        VkFormat&               GetFormat() { return Format; }
        
    private:

        VkSurfaceKHR                    Surface = VK_NULL_HANDLE;
        VkFormat                        Format;

        VkSwapchainKHR                  Swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR              SurfaceFormat;
        VkPresentModeKHR                CurrentPresentMode;
        
        TVector<VkSemaphore>            PresentSemaphores;
        TVector<VkSemaphore>            AquireSemaphores;
        TVector<VkFence>                Fences;
        
    };
}
