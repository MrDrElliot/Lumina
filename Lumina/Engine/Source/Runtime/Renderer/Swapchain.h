#pragma once

#include <vulkan/vulkan_core.h>

#include "Renderer.h"


namespace Lumina
{
    class FVulkanImage;

    struct FSwapchainSpec
    {
        FWindow* Window = nullptr;
        glm::ivec2 Extent;
        uint8 FramesInFlight;
    };

    struct FSwapchainSemaphores
    {
        VkSemaphore Render;
        VkSemaphore Present;
    };
    
    class FSwapchain : public RefCounted
    {
    public:

        FSwapchain() :Specifications({}), bDirty(false) {}
        
        static TRefPtr<FSwapchain> Create(const FSwapchainSpec& InSpec);

        virtual void CreateSurface(const FSwapchainSpec& InSpec) = 0;
        virtual void CreateSwapchain(const FSwapchainSpec& InSpec) = 0;
        virtual void CreateImages() = 0;

        virtual void DestroySurface() = 0;
        virtual void DestroySwapchain() = 0;
        virtual void RecreateSwapchain() = 0;
        
        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;

        inline bool WasSwapchainResizedThisFrame() const { return bWasResizedThisFrame; }
        inline bool IsSwapchainDirty() const { return bDirty; }
        inline void SetSwapchainDirty() { bDirty = true; }
        
        virtual uint32 GetCurrentFrameIndex() = 0;
        FSwapchainSpec& GetSpecs() { return Specifications; }

    protected:
        
        FSwapchainSpec Specifications;
        uint8 bDirty:1;
        uint8 bWasResizedThisFrame:1;

    };
}
