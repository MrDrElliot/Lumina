#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include "VkBootstrap.h"
#include "VulkanDescriptor.h"
#include "VulkanTypes.h"


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


        std::vector<VkImage> GetImages() { return SwapChain.get_images().value(); }
        std::vector<VkImageView> GetImageViews() { return SwapChain.get_image_views().value(); }
        
        
        FWindow* GetWindow() { return Window; }
        
        /* Get the extent from the actual swap chain's extent */
        VkExtent3D& GetExtent();
        VkExtent2D& GetExtent2D() { return SwapChain.extent; }

        VkFormat& GetFormat() { return ImageFormat; }
        
        VkSwapchainKHR& GetSwapChain() { return SwapChain.swapchain; }
        VkSurfaceKHR GetSurface() const { return Surface; }


        

        FAllocatedImage& GetDrawImage() { return DrawImage; }

        /* Get the extent from the draw image */
        VkExtent3D& GetDrawExtent() { return DrawImage.ImageExtent; }
        VkExtent2D& GetDrawExtent2D();

        
    private:
    
    private:

        bool bInitialized = false;

        vkb::Swapchain SwapChain;
        VkSurfaceKHR Surface;

        VkFormat ImageFormat;
        FAllocatedImage DrawImage;
        

        
        FWindow* Window;
    };
}
