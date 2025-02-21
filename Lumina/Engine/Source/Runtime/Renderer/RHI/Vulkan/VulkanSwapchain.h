#pragma once


#include "Source/Runtime/Renderer/Swapchain.h"
#include <vulkan/vulkan_core.h>
#include "VulkanSemaphore.h"

namespace Lumina
{
}

namespace Lumina
{
    class FVulkanSemaphore;
    
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
        void Present() override;

        VkSemaphore             GetAquireSemaphore() { return AquireSemaphores[CurrentFrameIndex]->GetVkSemaphore(); }
        VkSemaphore             GetPresentSemaphore() { return PresentSemaphores[CurrentImageIndex]->GetVkSemaphore(); }

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
        
        TVector<TRefCountPtr<FVulkanSemaphore>>       PresentSemaphores;
        TVector<TRefCountPtr<FVulkanSemaphore>>       AquireSemaphores;
        TVector<VkFence>                              Fences;
        
    };
}
