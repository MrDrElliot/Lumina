#include "RenderGraph.h"

#include "Core/Profiler/Profile.h"
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
        for (IRenderPass* Pass : RenderPasses)
        {
            Pass->Execute(CommandList);    
        }
    }

    void FRenderGraph::Compile()
    {
        
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
