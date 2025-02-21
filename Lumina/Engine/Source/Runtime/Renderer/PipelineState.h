#pragma once

#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Containers/Name.h"


namespace Lumina
{
    class FPipelineState
    {
    public:

        struct FPendingDescriptorWrite
        {
            FRHIDescriptorSet   DescriptorSet;
            FRHIBuffer          Buffer;
            uint8 DescriptorSetIndex = 0;
        };

        
        struct FPipelineStateBuffer
        {
            uint8 DescriptorSetIndex = 0;
            uint8 DescriptorIndex = 0;
            FRHIBuffer Buffer;
        };
        
        void SetPipeline(const FRHIPipeline& InPipeline);

        FORCEINLINE FRHIPipeline GetPipeline() const { return Pipeline; }

        void BindDescriptors();
        void FlushDescriptorWrites();

        void AddPendingDescriptorWrite(FRHIDescriptorSet Set, FRHIBuffer Buffer, uint8 DescriptorSetIndex);
        
        /** Mostly used for cleanup and to release resources */
        void ClearState();

        /** Get the buffer for a descriptor */
        FPipelineStateBuffer GetBufferForDescriptor(const FName& Descriptor);
        FRHIDescriptorSet GetDescriptorSetForDescriptor(const FName& Descriptor);
        
    protected:

        void BuildPipelineResources();

    private:

        /** Pipeline resource that is bound */
        FRHIPipeline                                        Pipeline;

        /** Shader resource that is bound to the pipeline */
        FRHIShader                                          Shader;

        /** DescriptorSet Index to set */
        THashMap<uint8, FRHIDescriptorSet>                  DescriptorSets[FRAMES_IN_FLIGHT];

        /** Descriptor key to descriptor index and buffer */
        THashMap<FName, FPipelineStateBuffer>               Buffers[FRAMES_IN_FLIGHT];

        TQueue<FPendingDescriptorWrite>                     PendingDescriptorWrites;
    };
}
