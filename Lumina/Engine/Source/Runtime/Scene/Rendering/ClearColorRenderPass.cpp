#include "ClearColorRenderPass.h"
#include "Renderer/RHIIncl.h"
#include "RenderGraphResources.h"
#include "Core/Profiler/Profile.h"
#include "Renderer/CommandList.h"

namespace Lumina
{
    void FClearColorRenderPass::Execute(FRenderGraphScope& Scope)
    {
        LUMINA_PROFILE_SCOPE();

        FRHIImageRef Image = Scope.Get<PrimaryRenderTargetTag>().As<FRHIImage>();
        Scope.GetCommandList()->ClearImageColor(Image, FColor::Black);
    }
    
}
