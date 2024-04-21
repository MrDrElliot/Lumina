#pragma once
#include <vulkan/vulkan_core.h>

#include "Source/Runtime/Renderer/Pipeline.h"

namespace Lumina
{
    class FVulkanPipeline : public FPipeline
    {
    public:

        FVulkanPipeline(const FPipelineSpecification& InSpec);
        ~FVulkanPipeline();

        const FPipelineSpecification& GetSpecification() const override { return Specification; }
        VkPipeline GetPipeline() { return Pipeline; }
        VkPipelineLayout GetPipelineLayout() { return PipelineLayout; }
        
        void Destroy() override;

        void CreateGraphics();
        void CreateCompute();


    private:
        
        FPipelineSpecification Specification;

        VkPipeline Pipeline;
        VkPipelineLayout PipelineLayout;
    };
}
