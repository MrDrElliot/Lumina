#include "VulkanPipelineCache.h"

#include "VulkanResources.h"

namespace Lumina
{

    FRHIGraphicsPipelineRef FVulkanPipelineCache::GetOrCreateGraphicsPipeline(FVulkanDevice* Device, const FGraphicsPipelineDesc& InDesc)
    {

        SIZE_T Hash = Hash::GetHash(InDesc);
        if (GraphicsPipelines.find(Hash) != GraphicsPipelines.end())
        {
            return GraphicsPipelines.at(Hash);
        }
        
        auto NewPipeline = MakeRefCount<FVulkanGraphicsPipeline>(Device, InDesc);

        GraphicsPipelines.emplace(Hash, NewPipeline);

        return NewPipeline;
    }

    FRHIComputePipelineRef FVulkanPipelineCache::GetOrCreateComputePipeline(FVulkanDevice* Device, const FComputePipelineDesc& InDesc)
    {
        SIZE_T Hash = Hash::GetHash(InDesc);
        if (ComputePipelines.find(Hash) != ComputePipelines.end())
        {
            return ComputePipelines.at(Hash);
        }
        
        auto NewPipeline = MakeRefCount<FVulkanComputePipeline>(Device, InDesc);

        ComputePipelines.emplace(Hash, NewPipeline);

        return NewPipeline;
    }

    void FVulkanPipelineCache::PostShaderRecompiled(FName Shader)
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
