#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include "VkBootstrap.h"


namespace Lumina
{
    class FWindow;

    class FVulkanSwapChain
    {
    public:

        FVulkanSwapChain();
        ~FVulkanSwapChain();
        static FVulkanSwapChain* Create();

        /* Swap Chains must be initialized with a GLFWwindow */
        void Init(FWindow* InWindow);
        
        void Present();

    public:
        
        VkSwapchainKHR GetSwapChain() const { return SwapChain; }
        VkSurfaceKHR GetSurface() const { return Surface; }
        
    private:

    private:

        bool bInitialized = false;

        vkb::Swapchain SwapChain;
        VkSurfaceKHR Surface;

        VkFormat ImageFormat;
        std::vector<VkImage> Images;
        std::vector<VkImageView> ImageViews;
        

        FWindow* Window;
    };
}
