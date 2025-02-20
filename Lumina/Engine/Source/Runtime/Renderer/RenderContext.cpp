#include "RenderContext.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{
    void IRenderContext::SetCurrentPipeline(FRHIPipeline Pipeline)
    {
        PipelineState.SetPipeline(Pipeline);
    }
}
