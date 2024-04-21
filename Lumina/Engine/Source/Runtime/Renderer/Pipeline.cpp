#include "Pipeline.h"

#include "PipelineLibrary.h"
#include "RHI/Vulkan/VulkanPipeline.h"

namespace Lumina
{
    std::shared_ptr<FPipeline> FPipeline::Create(const FPipelineSpecification& InSpec)
    {
        std::shared_ptr<FVulkanPipeline> NewPipeline = std::make_shared<FVulkanPipeline>(InSpec);
        FPipelineLibrary::AddPipeline(NewPipeline);
        return NewPipeline;
    }
}
