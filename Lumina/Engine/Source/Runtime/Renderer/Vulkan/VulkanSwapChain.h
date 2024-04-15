#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include <vk-bootstrap/src/VkBootstrap.h>
#include "VulkanTypes.h"


namespace Lumina
{
    class FVulkanRenderContext;
    class FWindow;

    class FVulkanSwapChain
    {
    public:

        FVulkanSwapChain();
        ~FVulkanSwapChain();
        static FVulkanSwapChain* Create();

        /* Swap Chains must be initialized with a GLFWwindow */
        void Init(FWindow* InWindow);
        void OnDraw();
        
        void Present();
        void Resize(uint32_t InWidth, uint32_t InHeight);

        void DestroySwapchain();

    public:


        std::vector<VkImage> GetImages() { return SwapChain.get_images().value(); }
        std::vector<VkImageView> GetImageViews() { return SwapChain.get_image_views().value(); }
        float GetAspectRatio() { return GetExtent2D().width / GetExtent2D().height; }
        
        FWindow* GetWindow() { return Window; }
        
        /* Get the extent from the actual swap chain's extent */
        VkExtent3D& GetExtent();
        VkExtent2D& GetExtent2D() { return SwapChain.extent; }

        VkFormat& GetFormat() { return ImageFormat; }
        
        VkSwapchainKHR& GetSwapChain() { return SwapChain.swapchain; }
        VkSurfaceKHR GetSurface() const { return Surface; }

        void CreateSwapChain(uint32_t NewWidth, uint32_t NewHeight);
        void CreateDevices();

        FAllocatedImage& GetDrawImage() { return DrawImage; }
        FAllocatedImage& GetDepthImage() { return DepthImage; }

        /* Get the extent from the draw image */
        VkExtent3D GetDrawExtent();
        VkExtent2D GetDrawExtent2D();

        void SetResizeRequested(bool bNew) { bResizeRequested = bNew; }
        bool IsResizeRequested() const { return bResizeRequested; }

        
    private:
    
    private:

        bool bInitialized = false;
        bool bResizeRequested = false;
        
        FVulkanRenderContext* VkRenderContext;
        
        vkb::Swapchain SwapChain;
        VkSurfaceKHR Surface;

        VkFormat ImageFormat;
        
        FAllocatedImage DrawImage;
        FAllocatedImage DepthImage;

        vkb::Device Device;

        float RenderScale = 1.0f;
        FWindow* Window;
    };
}
