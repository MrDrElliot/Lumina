#pragma once


#include "TrackedCommandBuffer.h"
#include "VulkanBarriers.h"
#include "VulkanPipelineStates.h"
#include "Renderer/CommandList.h"


namespace Lumina
{

    VkImageLayout ConvertRHIAccessToVkImageLayout(ERHIAccess Access);


    
    class FVulkanCommandList : public ICommandList
    {
    public:
        
        
        FVulkanCommandList(FVulkanRenderContext* InContext, const FCommandListInfo& InInfo)
            : GraphicsState()
            , ComputeState()
            , RenderContext(InContext)
            , Info(InInfo)
            , PushConstantVisibility(0)
            , CurrentPipelineLayout(nullptr)
        {
        }

        void Open() override;
        void Close() override;
        void Executed(FQueue* Queue) override;

        void CopyImage(FRHIImage* Src, FRHIImage* Dst) override;
        void WriteToImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, SIZE_T RowPitch, SIZE_T DepthPitch) override;

        void CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize) override;
        void UploadToBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Offset, uint32 Size) override;

        void SetRequiredImageAccess(FRHIImage* Image, ERHIAccess Access) override;
        void SetRequiredBufferAccess(FRHIBuffer* Buffer, ERHIAccess Access) override;
        void CommitBarriers() override;
        void SetResourceStatesForBindingSet(FRHIBindingSet* BindingSet) override;

        void AddMarker(const char* Name, const FColor& Color = FColor::Red) override;
        void PopMarker() override;

        void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass() override;
        void ClearImageColor(FRHIImage* Image, const FColor& Color) override;

        void BindBindingSets(TVector<FRHIBindingSet*> BindingSets, ERHIBindingPoint BindPoint) override;

        void SetPushConstants(const void* Data, SIZE_T ByteSize) override;

        void BindVertexBuffer(FRHIBuffer* Buffer, uint32 Index, uint32 Offset) override;
        void SetGraphicsPipeline(FRHIGraphicsPipeline* InPipeline) override;
        void SetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) override;
        void SetScissorRect(uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) override;
        void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) override;
        void DrawIndexed(FRHIBuffer* IndexBuffer, uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance) override;

        void SetComputePipeline(FRHIComputePipeline* InPipeline) override;
        void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) override;



        
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        const FCommandListInfo& GetCommandListInfo() const override { return Info; }

        FPendingCommandState& GetPendingCommandState() override { return PendingState; }

        TRefCountPtr<FTrackedCommandBuffer>  CurrentCommandBuffer;

    private:
        
        FVulkanGraphicsState            GraphicsState;
        FVulkanComputeState             ComputeState;

        FCommandListStateTracker        CommandListTracker;
        FPendingCommandState            PendingState;
        FVulkanRenderContext*           RenderContext = nullptr;
        FCommandListInfo                Info;
        VkShaderStageFlags              PushConstantVisibility;
        VkPipelineLayout                CurrentPipelineLayout;
    };
    
}
