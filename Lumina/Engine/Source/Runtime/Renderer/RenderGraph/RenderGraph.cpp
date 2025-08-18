#include "RenderGraph.h"

#include "RenderGraphPass.h"
#include "RenderGraphResources.h"
#include "Core/Engine/Engine.h"
#include "Renderer/RHIGlobals.h"


namespace Lumina
{
    constexpr size_t InitialLinearAllocatorSize = 1024;
    
    FRenderGraph::FRenderGraph()
        :GraphAllocator(InitialLinearAllocatorSize)
    {
        GraphicsCommandList = GRenderContext->GetCommandList(ECommandQueue::Graphics);
        ComputeCommandList = GRenderContext->GetCommandList(ECommandQueue::Compute);
        TransferCommandList = GRenderContext->GetCommandList(ECommandQueue::Transfer);
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
