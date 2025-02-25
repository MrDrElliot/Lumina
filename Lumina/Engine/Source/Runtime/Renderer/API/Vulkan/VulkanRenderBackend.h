#pragma once
#include "Renderer/RenderBackend.h"

#ifdef LUMINA_RENDERER_VULKAN

namespace Lumina
{
    class FVulkanRenderBackend : public IRenderBackend
    {
    public:
        
        void Initialize() override;

        VkInstance GetVulkanInstance() const;


    private:
        
        VkInstance             VulkanInstance;
        VkSwapchainKHR         Swapchain;
    };
    
}

#endif