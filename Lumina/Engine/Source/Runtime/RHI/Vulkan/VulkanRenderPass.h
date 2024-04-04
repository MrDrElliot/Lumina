#pragma once

#include "VulkanSwapChain.h"

class FVulkanSwapChain;

namespace Lumina
{
    class FVulkanRenderPass
    {
    public:

        FVulkanRenderPass(VkDevice InDevice, FVulkanSwapChain* InSwapChain);
        ~FVulkanRenderPass();

        VkRenderPass GetRenderPass() const { return RenderPass; }
        VkPipelineLayout GetPipelineLayout() const { return PipelineLayout; }

    private:

        VkRenderPass RenderPass;
        VkPipelineLayout PipelineLayout;
        FVulkanSwapChain* SwapChain;
        VkDevice Device;
        
    };
}
