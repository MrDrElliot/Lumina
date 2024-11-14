#include "Pipeline.h"

#include "PipelineLibrary.h"
#include "RHI/Vulkan/VulkanPipeline.h"

namespace Lumina
{
    TRefPtr<FPipeline> FPipeline::Create(const FPipelineSpecification& InSpec)
    {
        TRefPtr<FVulkanPipeline> NewPipeline = MakeRefPtr<FVulkanPipeline>(InSpec);
        FPipelineLibrary::AddPipeline(NewPipeline);
        return NewPipeline;
    }
}
