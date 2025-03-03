﻿#pragma once

#include "TrackedCommandBuffer.h"
#include "VulkanBarriers.h"
#include "VulkanPendingState.h"
#include "Renderer/CommandList.h"


namespace Lumina
{

    VkImageLayout ConvertRHIAccessToVkImageLayout(ERHIAccess Access);


    
    class FVulkanCommandList : public ICommandList
    {
    public:
        
        
        FVulkanCommandList(FVulkanRenderContext* InContext, const FCommandListInfo& InInfo)
            : RenderContext(InContext)
            , Info(InInfo)
        {}

        void Open() override;
        void Close() override;
        void Executed(FQueue* Queue) override;

        void CopyImage(FRHIImage* Src, FRHIImage* Dst) override;
        void WriteToImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, size_t RowPitch, size_t DepthPitch) override;

        void CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize) override;
        void UploadToBuffer(FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size) override;

        void SetRequiredImageAccess(FRHIImageRef Image, ERHIAccess Access) override;
        void CommitBarriers() override;

        void AddMarker(const char* Name) override;
        void PopMarker() override;

        void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass() override;
        void ClearImageColor(FRHIImage* Image, const FColor& Color) override;
        void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) override;
        void DrawIndexed(uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance) override;
        void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) override;

        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        const FCommandListInfo& GetCommandListInfo() const override { return Info; }

        FPendingCommandState& GetPendingCommandState() override { return PendingState; }

        TRefCountPtr<FTrackedCommandBufer>  CurrentCommandBuffer;

    private:

        FVulkanPendingComputeState      PendingComputeState;
        FVulkanPendingGraphicsState     PendingGraphicsState;

        FCommandListStateTracker        CommandListTracker;
        FPendingCommandState            PendingState;
        FVulkanRenderContext*           RenderContext = nullptr;
        FCommandListInfo                Info;
    };
}
