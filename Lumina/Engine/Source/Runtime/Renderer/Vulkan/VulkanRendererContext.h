#pragma once

#include "Source/Runtime/Renderer/RendererContext.h"
#include <vulkan/vulkan.h>


namespace Lumina
{
    class FVulkanPhysicalDevice;
    class FVulkanLogicalDevice;
}

namespace Lumina
{
    class FVulkanRendererContext : public FRendererContext
    {
    public:

        FVulkanRendererContext();
        ~FVulkanRendererContext();

        void Init() override;

        

    private:

        FVulkanLogicalDevice* LogicalDevice;
        FVulkanPhysicalDevice* PhysicalDevice;

        static VkInstance Instance;
    
    };
}
