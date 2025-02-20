#pragma once

#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Containers/Name.h"


namespace Lumina
{
    class FPipelineState
    {
    public:

        void SetPipeline(const FRHIPipeline& InPipeline);

    protected:

        void BuildPipelineResources();

    private:

        /** Pipeline resource that is bound */
        FRHIPipeline                                        Pipeline;

        /** Shader resource that is bound to the pipeline */
        FRHIShader                                          Shader;

        /** Descriptor Index to set */
        THashMap<uint8, FRHIDescriptorSet>                  DescriptorSets;

        /** Descriptor index to location and buffer */
        THashMap<uint8, TPair<FName, FRHIBuffer>>           Buffers;
    };
}
