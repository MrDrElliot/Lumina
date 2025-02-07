#pragma once

#include <vulkan/vulkan_core.h>
#include "Source/Runtime/Renderer/Pipeline.h"

namespace Lumina
{
    class FVulkanPipeline : public FPipeline
    {
    public:

        FVulkanPipeline(const FPipelineSpecification& InSpec);
        ~FVulkanPipeline() override;

        VkPipeline GetPipeline() { return Pipeline; }
        VkPipelineLayout GetPipelineLayout() { return PipelineLayout; }

        void SetFriendlyName(const FString& InName) override;

        void CreateGraphics() override;
        void CreateCompute() override;

    private:
        
        VkPipeline Pipeline;
        VkPipelineLayout PipelineLayout;
    };
}
