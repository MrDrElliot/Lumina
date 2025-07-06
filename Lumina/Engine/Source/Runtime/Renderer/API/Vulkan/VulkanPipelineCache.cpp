#include "VulkanPipelineCache.h"

#include "VulkanResources.h"
#include "Core/Profiler/Profile.h"

namespace Lumina
{

    FRHIGraphicsPipelineRef FVulkanPipelineCache::GetOrCreateGraphicsPipeline(FVulkanDevice* Device, const FGraphicsPipelineDesc& InDesc)
    {
        LUMINA_PROFILE_SCOPE();

        SIZE_T Hash = Hash::GetHash(InDesc);
        if (GraphicsPipelines.find(Hash) != GraphicsPipelines.end())
        {
            return GraphicsPipelines.at(Hash);
        }
        
        auto NewPipeline = TRefCountPtr<FVulkanGraphicsPipeline>::Create(Device, InDesc);
        

        GraphicsPipelines.emplace(Hash, NewPipeline);
        return NewPipeline;
    }

    FRHIComputePipelineRef FVulkanPipelineCache::GetOrCreateComputePipeline(FVulkanDevice* Device, const FComputePipelineDesc& InDesc)
    {
        LUMINA_PROFILE_SCOPE();

        SIZE_T Hash = Hash::GetHash(InDesc);
        if (ComputePipelines.find(Hash) != ComputePipelines.end())
        {
            return ComputePipelines.at(Hash);
        }
        
        auto NewPipeline = MakeRefCount<FVulkanComputePipeline>(Device, InDesc);
        
        ComputePipelines.emplace(Hash, NewPipeline);
        return NewPipeline;
    }

    void FVulkanPipelineCache::PostShaderRecompiled(const IVulkanShader* Shader)
    {
        GraphicsPipelines.clear();
        ComputePipelines.clear();
    }
    
    void FVulkanPipelineCache::ReleasePipelines()
    {
        GraphicsPipelines.clear();
        ComputePipelines.clear();
    }
}
