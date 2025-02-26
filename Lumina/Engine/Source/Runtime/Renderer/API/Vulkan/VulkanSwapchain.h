#pragma once

#ifdef LUMINA_RENDERER_VULKAN
#include "Containers/Array.h"
#include <vulkan/vulkan_core.h>

namespace Lumina
{
    class FVulkanRenderContext;
    class FWindow;
}

namespace Lumina
{

    class FVulkanSwapchain
    {
    public:

        ~FVulkanSwapchain();

        void CreateSwapchain(VkInstance Instance, FVulkanRenderContext* Device, FWindow* Window, FVector2D Extent, bool bFromResize = false);

        FORCEINLINE VkFormat GetSwapchainFormat() const { return Format; }
        
        FRHIImageHandle GetCurrentImage() const;
        
    private:

        VkSurfaceKHR                    Surface;
        VkFormat                        Format;

        VkSwapchainKHR                  Swapchain;
        VkSurfaceFormatKHR              SurfaceFormat;
        VkPresentModeKHR                CurrentPresentMode;
        
        TVector<FRHIImageHandle>        SwapchainImages;
        TVector<VkSemaphore>            PresentSemaphores;
        TVector<VkSemaphore>            AquireSemaphores;
        TVector<VkFence>                Fences;
    };
}

#endif