#include "RenderGraph.h"

#include "RenderGraphPass.h"
#include "RenderGraphResources.h"
#include "Core/Engine/Engine.h"
#include "Renderer/RenderManager.h"


namespace Lumina
{
    constexpr size_t InitialLinearAllocatorSize = 1 * 1024 * 1024; // 1 MB
    
    FRenderGraph::FRenderGraph()
        :GraphAllocator(InitialLinearAllocatorSize)
    {
        RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        GraphicsCommandList = RenderContext->GetCommandList(ECommandQueue::Graphics);
        ComputeCommandList = RenderContext->GetCommandList(ECommandQueue::Compute);
        TransferCommandList = RenderContext->GetCommandList(ECommandQueue::Transfer);
    }

    void FRenderGraph::Execute()
    {

        
        AllocateTransientResources();
        
        for (FRGPassHandle Pass : Passes)
        {
            EPipelineType Type = Pass->GetPipelineType();
            FRHICommandListRef CommandList = Type == EPipelineType::Compute ? ComputeCommandList : GraphicsCommandList;
            Pass->Execute(*CommandList);
        }
    }

    void FRenderGraph::Compile()
    {
    }

    void FRenderGraph::AllocateTransientResources()
    {
        for (const FRGBuffer* Buffer : BufferRegistry)
        {

        }

        for (const FRGImage* Image : ImageRegistry)
        {

        }
    }

    FRGBuffer* FRenderGraph::CreateBuffer(const FRHIBufferDesc& Desc)
    {
        return BufferRegistry.Allocate(GraphAllocator);
    }

    FRGImage* FRenderGraph::CreateImage(const FRHIImageDesc& Desc)
    {
        return ImageRegistry.Allocate(GraphAllocator);
    }
}
