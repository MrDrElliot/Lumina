#pragma once
#include "VulkanResources.h"

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

        void CreateSwapchain(VkInstance Instance, FVulkanRenderContext* InContext, FWindow* Window, FIntVector2D Extent, bool bFromResize = false);

        void RecreateSwapchain(const FIntVector2D& Extent);
        void SetPresentMode(VkPresentModeKHR NewMode);
        
        FORCEINLINE VkPresentModeKHR GetPresentMode() const { return CurrentPresentMode; }
        FORCEINLINE VkFormat GetSwapchainFormat() const { return Format; }
        FORCEINLINE const FIntVector2D& GetSwapchainExtent() const { return SwapchainExtent; }
        
        TRefCountPtr<FVulkanImage> GetCurrentImage() const;

        bool AcquireNextImage();
        bool Present();
        
    private:

        uint64                                  AcquireSemaphoreIndex = 0;
        uint32                                  CurrentImageIndex = 0;
        
        bool                                    bNeedsResize = false;
        VkSurfaceKHR                            Surface = nullptr;
        VkFormat                                Format = VK_FORMAT_MAX_ENUM;
        FIntVector2D                            SwapchainExtent;
                                                
        VkSwapchainKHR                          Swapchain = nullptr;
        VkSurfaceFormatKHR                      SurfaceFormat = {};
        VkPresentModeKHR                        CurrentPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        
        TVector<TRefCountPtr<FVulkanImage>>     SwapchainImages;
        TVector<VkSemaphore>                    PresentSemaphores;
        TVector<VkSemaphore>                    AcquireSemaphores;
        FVulkanRenderContext*                   Context = nullptr;

        TQueue<FRHIEventQueryRef>               FramesInFlight;
        TVector<FRHIEventQueryRef>              QueryPool;
    };
    
}

#endif