#pragma once

#include <vulkan/vulkan_core.h>

#include "Renderer.h"


namespace Lumina
{
    class FVulkanImage;

    struct FSwapchainSpec
    {
        FWindow* Window;
        int FramesInFlight;
        glm::ivec2 Extent;
    };

    struct FSwapchainSemaphores
    {
        VkSemaphore Render;
        VkSemaphore Present;
    };
    
    class FSwapchain
    {
    public:

        
        static std::shared_ptr<FSwapchain> Create(const FSwapchainSpec& InSpec);

        virtual void CreateSurface(const FSwapchainSpec& InSpec) = 0;
        virtual void CreateSwapchain(const FSwapchainSpec& InSpec) = 0;

        virtual void DestroySurface() = 0;
        virtual void DestroySwapchain() = 0;
        
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        FSwapchainSpec& GetSpecs() { return Specifications; }

    protected:
        
        FSwapchainSpec Specifications;
    

        
    private:


        
    };
}
