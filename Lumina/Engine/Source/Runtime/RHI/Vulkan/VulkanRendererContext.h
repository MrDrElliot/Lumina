#pragma once

#include "Source/Runtime/RHI/RendererContext.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Lumina
{
    class LVulkanRendererContext : public LRendererContext
    {
    public:

        
        
        LVulkanRendererContext();
        ~LVulkanRendererContext();
        
        void Init() override;
        
    private:
        
        VkInstance Instance;
    };
}
