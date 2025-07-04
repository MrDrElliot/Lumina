#pragma once
#include "Containers/Array.h"
#include "Renderer/RHIFwd.h"


namespace Lumina
{
    class FName;
    struct FComputePipelineDesc;
    struct FGraphicsPipelineDesc;
    class FVulkanDevice;
}


namespace Lumina
{

    class FVulkanPipelineCache
    {
    public:

        struct FShaderPipelineTracker
        {
            TVector<FName> Shaders;
        };

        FRHIGraphicsPipelineRef GetOrCreateGraphicsPipeline(FVulkanDevice* Device, const FGraphicsPipelineDesc& InDesc);
        FRHIComputePipelineRef GetOrCreateComputePipeline(FVulkanDevice* Device, const FComputePipelineDesc& InDesc);

        void PostShaderRecompiled(const FName& Shader);
        void ReleasePipelines();
        
    private:

        THashMap<SIZE_T, FRHIGraphicsPipelineRef>   GraphicsPipelines;
        THashMap<SIZE_T, FRHIComputePipelineRef>    ComputePipelines;
        
    };
}
