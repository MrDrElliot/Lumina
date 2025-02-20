#pragma once

#include <vulkan/vulkan_core.h>
#include "Source/Runtime/Renderer/Pipeline.h"

namespace Lumina
{
    
    class FVulkanPipeline : public FPipeline
    {
    public:

        
        FVulkanPipeline(const FPipelineSpec& PipelineSpec);
        ~FVulkanPipeline() override;

        void* GetPlatformPipeline() const override { return Pipeline; }
        void* GetPlatformPipelineLayout() const override { return PipelineLayout; }

        void SetFriendlyName(const FString& InName) override;

        void CreateGraphics() override;
        void CreateCompute() override;

    private:
        
        VkPipeline Pipeline;
        VkPipelineLayout PipelineLayout;
    };
}
