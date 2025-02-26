#pragma once

#ifdef LUMINA_RENDERER_VULKAN
#include "Containers/Array.h"
#include <vulkan/vulkan_core.h>
#include "Core/Math/Math.h"
#include "Renderer/RHIFwd.h"

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

        void CreateSwapchain(VkInstance Instance, FVulkanRenderContext* Device, FWindow* Window, FIntVector2D Extent, bool bFromResize = false);

        void RecreateSwapchain(const FIntVector2D& Extent);
        void SetPresentMode(VkPresentModeKHR NewMode);
        
        FORCEINLINE VkPresentModeKHR GetPresentMode() const { return CurrentPresentMode; }
        FORCEINLINE VkFormat GetSwapchainFormat() const { return Format; }
        FORCEINLINE VkSemaphore GetPresentSemaphore() const { return PresentSemaphores[CurrentImageIndex]; }
        FORCEINLINE VkSemaphore GetAquireSemaphore() const { return AquireSemaphores[CurrentFrameIndex]; }
        FORCEINLINE VkFence GetFence() const { return Fences[CurrentFrameIndex]; }
        FORCEINLINE const FIntVector2D& GetSwapchainExtent() const { return SwapchainExtent; }
        
        FRHIImageHandle GetCurrentImage() const;

        void AquireNextImage(uint32 NewFrameIndex);
        void Present();
        
    private:
        
        bool                            bNeedsResize = false;
        uint32                          CurrentFrameIndex = 0;
        uint32                          CurrentImageIndex = 0;
        VkSurfaceKHR                    Surface;
        VkFormat                        Format;
        FIntVector2D                    SwapchainExtent;

        VkSwapchainKHR                  Swapchain;
        VkSurfaceFormatKHR              SurfaceFormat;
        VkPresentModeKHR                CurrentPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        
        TVector<FRHIImageHandle>        SwapchainImages;
        TVector<VkSemaphore>            PresentSemaphores;
        TVector<VkSemaphore>            AquireSemaphores;
        TVector<VkFence>                Fences;
        FVulkanRenderContext*           Context = nullptr;
    };
    
}

#endif