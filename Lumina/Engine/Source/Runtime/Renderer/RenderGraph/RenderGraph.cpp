#include "RenderGraph.h"
#include "Renderer/RHIIncl.h"
#include "Core/Profiler/Profile.h"
#include "EASTL/sort.h"
#include "RenderPasses/RenderPass.h"

namespace Lumina
{
    void FRenderGraph::ClearPasses()
    {
        for (IRenderPass* Pass : RenderPasses)
        {
            Memory::Delete(Pass);
        }
        
        RenderPasses.clear();
    }

    void FRenderGraph::Execute(ICommandList* CommandList)
    {
        LUMINA_PROFILE_SCOPE();
        
        FRenderGraphScope Scope;
        Scope.CommandList = CommandList;
        Scope.RenderContext = RenderContext;
        Scope.Resources = Resources;
        
        for (IRenderPass* Pass : RenderPasses)
        {
            Pass->Execute(Scope);    
        }
    }

    void FRenderGraph::Compile()
    {
        LUMINA_PROFILE_SCOPE();
    }


    void FRenderGraph::AddPassInternal(IRenderPass* InPass)
    {
        for (IRenderPass* Pass : RenderPasses)
        {
            if (Pass->GetPassName() == InPass->GetPassName())
            {
                LOG_ERROR("Render Pass \"{}\" already exists in render graph!", InPass->GetPassName());
                break;
            }
        }

        RenderPasses.push_back(InPass);
    }
}
