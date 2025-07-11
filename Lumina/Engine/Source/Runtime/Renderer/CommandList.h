#pragma once

#include "PendingState.h"
#include "RenderResource.h"
#include "Core/Math/Color.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FQueue;
}

namespace Lumina
{
    struct FRenderPassBeginInfo;
    class IRenderContext;
}

namespace Lumina
{

    enum class ECommandQueue : uint8;

    struct FCommandListInfo
    {
        
        /** Type of command queue that this list is to be executed on */
        ECommandQueue       CommandQueue = ECommandQueue::Graphics;
        
    };
    
    class ICommandList : public IRHIResource
    {
    public:

        RENDER_RESOURCE(RRT_CommandList)

        ICommandList() = default;
        virtual ~ICommandList() override = default;

        virtual void Open() = 0;
        virtual void Close() = 0;
        virtual void Executed(FQueue* Queue, uint64 SubmissionID) = 0;

        virtual void CopyImage(FRHIImage* Src, FRHIImage* Dst) = 0;
        virtual void WriteImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, SIZE_T RowPitch, SIZE_T DepthPitch) = 0;
        
        NODISCARD virtual void WriteBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Offset, uint32 Size) = 0;
        virtual void CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize) = 0;

        virtual void SetRequiredImageAccess(FRHIImage* Image, ERHIAccess Access) = 0;
        virtual void SetRequiredBufferAccess(FRHIBuffer* Buffer, ERHIAccess Access) = 0;
        virtual void CommitBarriers() = 0;
        virtual void SetResourceStatesForBindingSet(FRHIBindingSet* BindingSet) = 0;

        virtual void AddMarker(const char* Name, const FColor& Color = FColor::Red) = 0;
        virtual void PopMarker() = 0;
        
        virtual void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass() = 0;
        virtual void ClearImageColor(FRHIImage* Image, const FColor& Color) = 0;

        virtual void BindBindingSets(TVector<FRHIBindingSet*> BindingSets, ERHIBindingPoint BindPoint) = 0;

        virtual void SetPushConstants(const void* Data, SIZE_T ByteSize) = 0;
        
        virtual void BindVertexBuffer(FRHIBuffer* Buffer, uint32 Index, uint32 Offset) = 0;
        virtual void SetGraphicsPipeline(FRHIGraphicsPipeline* InPipeline) = 0;
        virtual void SetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) = 0;
        virtual void SetScissorRect(uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) = 0;
        virtual void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) = 0;
        virtual void DrawIndexed(FRHIBuffer* IndexBuffer, uint32 IndexCount, uint32 InstanceCount = 1, uint32 FirstIndex = 1, int32 VertexOffset = 0, uint32 FirstInstance = 0) = 0;

        
        virtual void SetComputePipeline(FRHIComputePipeline* InPipeline) = 0;
        virtual void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) = 0;

        NODISCARD virtual const FCommandListInfo& GetCommandListInfo() const = 0;
        NODISCARD virtual FPendingCommandState& GetPendingCommandState() = 0;
    
    protected:
        
    };
}
