#include "PipelineLibrary.h"
#include "Renderer/RHIIncl.h"


namespace Lumina
{
    void FPipelineLibrary::Shutdown()
    {
        PipelineMap.clear();
    }

    FRHIPipeline FPipelineLibrary::GetOrCreatePipeline(const FPipelineSpec& PipelineSpec)
    {
        uint64 Hash = PipelineSpec.GetHash();

        if (PipelineMap.find(Hash) != PipelineMap.end())
        {
            return PipelineMap[Hash];
        }

        FRHIPipeline NewPipeline =  FPipeline::Create(PipelineSpec);
        PipelineMap.emplace(Hash, NewPipeline);

        return NewPipeline;
    }
}
