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

        // Minimum size of memory chunks created to upload data to the device on DX12.
        SIZE_T UploadChunkSize = 64 * 1024;

        // Minimum size of memory chunks created for AS build scratch buffers.
        SIZE_T ScratchChunkSize = 64 * 1024;

        // Maximum total memory size used for all AS build scratch buffers owned by this command list.
        SIZE_T ScratchMaxMemory = 1024 * 1024 * 1024;
        
        /** Type of command queue that this list is to be executed on */
        ECommandQueue CommandQueue = ECommandQueue::Graphics;

        static FCommandListInfo Transfer()
        {
            FCommandListInfo Ret;
            Ret.CommandQueue = ECommandQueue::Transfer;
            return Ret;
        }

        static FCommandListInfo Graphics()
        {
            FCommandListInfo Ret;
            Ret.CommandQueue = ECommandQueue::Graphics;
            return Ret;
        }

        static FCommandListInfo Compute()
        {
            FCommandListInfo Ret;
            Ret.CommandQueue = ECommandQueue::Compute;
            return Ret;
        }
        
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


        virtual void CopyImage(FRHIImage* Src, const FTextureSlice& SrcSlice, FRHIImage* Dst, const FTextureSlice& DstSlice) = 0;
        virtual void WriteImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, SIZE_T RowPitch, SIZE_T DepthPitch) = 0;
        
        NODISCARD virtual void WriteBuffer(FRHIBuffer* Buffer, const void* Data, SIZE_T Offset, SIZE_T Size) = 0;
        virtual void CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize) = 0;
        
        virtual void SetPermanentImageState(FRHIImage* Image, EResourceStates StateBits) = 0;
        virtual void SetPermanentBufferState(FRHIBuffer* Buffer, EResourceStates StateBits) = 0;
        
        virtual void BeginTrackingImageState(FRHIImage* Image, FTextureSubresourceSet Subresources, EResourceStates StateBits) = 0;
        virtual void BeginTrackingBufferState(FRHIBuffer* Buffer, EResourceStates StateBits) = 0;
        
        virtual void SetImageState(FRHIImage* Image, FTextureSubresourceSet Subresources, EResourceStates StateBits) = 0;
        virtual void SetBufferState(FRHIBuffer* Buffer, EResourceStates StateBits) = 0;
        
        virtual void SetResourceStatesForBindingSet(FRHIBindingSet* BindingSet) = 0;
        
        virtual void CommitBarriers() = 0;

        virtual EResourceStates GetImageSubresourceState(FRHIImage* Image, uint32 ArraySlice, uint32 MipLevel) = 0;
        virtual EResourceStates GetBufferState(FRHIBuffer* Buffer) = 0; 

        virtual void AddMarker(const char* Name, const FColor& Color = FColor::Red) = 0;
        virtual void PopMarker() = 0;
        
        virtual void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass() = 0;
        virtual void ClearImageColor(FRHIImage* Image, const FColor& Color) = 0;
        
        virtual void SetPushConstants(const void* Data, SIZE_T ByteSize) = 0;
        
        virtual void SetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) = 0;
        virtual void SetScissorRect(uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) = 0;
        virtual void SetGraphicsState(const FGraphicsState& State) = 0;
        virtual void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) = 0;
        virtual void DrawIndexed(uint32 IndexCount, uint32 InstanceCount = 1, uint32 FirstIndex = 1, int32 VertexOffset = 0, uint32 FirstInstance = 0) = 0;
        virtual void DrawIndirect(uint32 DrawCount, uint64 Offset) = 0;
        virtual void DrawIndexedIndirect(uint32 DrawCount, uint64 Offset) = 0;
        
        virtual void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) = 0;

        NODISCARD virtual const FCommandListInfo& GetCommandListInfo() const = 0;
        NODISCARD virtual FPendingCommandState& GetPendingCommandState() = 0;
    
    protected:
        
    };
}
