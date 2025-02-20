#include "PipelineLibrary.h"
#include "Renderer/RHIIncl.h"


namespace Lumina
{
    TRefCountPtr<FPipeline> FPipelineLibrary::GetOrCreatePipeline(const FPipelineSpec& PipelineSpec)
    {
        uint64 Hash = PipelineSpec.GetHash();

        if (PipelineMap.find(Hash) != PipelineMap.end())
        {
            return PipelineMap[Hash];
        }

        TRefCountPtr<FPipeline> NewPipeline =  FPipeline::Create(PipelineSpec);
        PipelineMap.emplace(Hash, NewPipeline);

        return NewPipeline;
    }
}
