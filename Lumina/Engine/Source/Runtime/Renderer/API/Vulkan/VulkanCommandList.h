#pragma once


#include "TrackedCommandBuffer.h"
#include "VulkanPipelineStates.h"
#include "Renderer/CommandList.h"


namespace Lumina
{

    VkImageLayout ConvertRHIAccessToVkImageLayout(ERHIAccess Access);

    struct FResourceStateMapping
    {
        EResourceStates State;
        VkPipelineStageFlags StageFlags;
        VkAccessFlags AccessMask;
        VkImageLayout ImageLayout;
        FResourceStateMapping(EResourceStates InState, VkPipelineStageFlags InStageFlags, VkAccessFlags InAccessMask, VkImageLayout InImageLayout):
            State(InState), StageFlags(InStageFlags), AccessMask(InAccessMask), ImageLayout(InImageLayout)
        {}
        
    };

    struct FResourceStateMapping2 // for use with KHR_synchronization2
    {
            EResourceStates State;
            VkPipelineStageFlags2 stageFlags;
            VkAccessFlags2 accessMask;
            VkImageLayout imageLayout;
            FResourceStateMapping2(EResourceStates InState, VkPipelineStageFlags2 InStageFlags, VkAccessFlags2 InAccessMask, VkImageLayout ImageLayout) :
                State(InState), stageFlags(InStageFlags), accessMask(InAccessMask), imageLayout(ImageLayout)
        {}
    };
    
    class FVulkanCommandList : public ICommandList
    {
    public:
        
        
        FVulkanCommandList(FVulkanRenderContext* InContext, const FCommandListInfo& InInfo)
            : UploadManager(MakeUniquePtr<FUploadManager>(InContext, InInfo.UploadChunkSize, 0, false))
            , ComputeState()
            , RenderContext(InContext)
            , Info(InInfo)
            , PushConstantVisibility(0)
            , CurrentPipelineLayout(nullptr)
        {
        }

        void Open() override;
        void Close() override;
        void Executed(FQueue* Queue, uint64 SubmissionID) override;

        
        void CopyImage(FRHIImage* Src, const FTextureSlice& SrcSlice, FRHIImage* Dst, const FTextureSlice& DstSlice) override;
        void WriteImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, SIZE_T RowPitch, SIZE_T DepthPitch) override;

        void CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize) override;
        void WriteBuffer(FRHIBuffer* Buffer, const void* Data, SIZE_T Offset, SIZE_T Size) override;

        void UpdateGraphicsDynamicBuffers();
        void FlushDynamicBufferWrites();
        void SubmitDynamicBuffers(uint64 RecordingID, uint64 SubmittedID);

        void SetPermanentImageState(FRHIImage* Image,EResourceStates StateBits) override;
        void SetPermanentBufferState(FRHIBuffer* Buffer, EResourceStates StateBits) override;

        void BeginTrackingImageState(FRHIImage* Image, FTextureSubresourceSet Subresources, EResourceStates StateBits) override;
        void BeginTrackingBufferState(FRHIBuffer* Buffer, EResourceStates StateBits) override;

        void SetImageState(FRHIImage* Image, FTextureSubresourceSet Subresources, EResourceStates StateBits) override;
        void SetBufferState(FRHIBuffer* Buffer, EResourceStates StateBits) override;
        
        EResourceStates GetImageSubresourceState(FRHIImage* Image, uint32 ArraySlice, uint32 MipLevel) override;
        EResourceStates GetBufferState(FRHIBuffer* Buffer) override;
        
        void CommitBarriers() override;
        void SetResourceStatesForBindingSet(FRHIBindingSet* BindingSet) override;
        void SetResourceStateForRenderPass(const FRenderPassBeginInfo& PassInfo) override;

        void AddMarker(const char* Name, const FColor& Color = FColor::Red) override;
        void PopMarker() override;

        void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass() override;
        
        void ClearImageColor(FRHIImage* Image, const FColor& Color) override;

        void BindBindingSets(VkPipelineBindPoint BindPoint, VkPipelineLayout PipelineLayout, TVector<FRHIBindingSet*> BindingSets);

        void SetPushConstants(const void* Data, SIZE_T ByteSize) override;

        void SetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) override;
        void SetScissorRect(uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) override;
        void SetGraphicsState(const FGraphicsState& State) override;
        
        void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) override;
        void DrawIndexed(uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance) override;
        void DrawIndirect(uint32 DrawCount, uint64 Offset) override;
        void DrawIndexedIndirect(uint32 DrawCount, uint64 Offset) override;

        void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) override;


        void CommitBarriersInternal();

        void TrackResourcesAndBarriers(const FGraphicsState& State);

        void RequireTextureState(FRHIImage* Texture, FTextureSubresourceSet Subresources, EResourceStates StateBits);

        void RequireBufferState(FRHIBuffer* Buffer, EResourceStates StateBits);
        
        
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        const FCommandListInfo& GetCommandListInfo() const override { return Info; }

        FPendingCommandState& GetPendingCommandState() override { return PendingState; }

        TRefCountPtr<FTrackedCommandBuffer>  CurrentCommandBuffer;

    private:

        TUniquePtr<FUploadManager>                      UploadManager;
        
        THashMap<FRHIBufferRef, FDynamicBufferWrite>    DynamicBufferWrites;
        bool                                            bHasDynamicBufferWrites = false;

        FGraphicsState                                  CurrentGraphicsState;
        FVulkanComputeState                             ComputeState;
                                                        
        FCommandListResourceStateTracker                StateTracker;
        FPendingCommandState                            PendingState;
        FVulkanRenderContext*                           RenderContext = nullptr;
        FCommandListInfo                                Info;
        VkShaderStageFlags                              PushConstantVisibility;
        VkPipelineLayout                                CurrentPipelineLayout;
    };
    
}
