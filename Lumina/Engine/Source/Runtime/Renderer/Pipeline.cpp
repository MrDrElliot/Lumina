#include "Pipeline.h"

#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanPipeline.h"

namespace Lumina
{
    FRHIPipeline FPipeline::Create(const FPipelineSpec& PipelineSpec)
    {
        return FRHIPipeline(MakeRefCount<FVulkanPipeline>(PipelineSpec));
    }
    
}
