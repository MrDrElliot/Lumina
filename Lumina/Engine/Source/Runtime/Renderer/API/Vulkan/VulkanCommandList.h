#pragma once

#include "TrackedCommandBuffer.h"
#include "Renderer/CommandList.h"



namespace Lumina
{
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

        void CopyBuffer(FRHIBuffer* Source, FRHIBuffer* Destination) override;
        void UploadToBuffer(FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size) override;


        void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass() override;
        void ClearColor(const FColor& Color) override;
        void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) override;
        void DrawIndexed(uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance) override;
        void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) override;

        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        const FCommandListInfo& GetCommandListInfo() const override { return Info; }

        TRefCountPtr<FTrackedCommandBufer>  CurrentCommandBuffer;

    private:

        bool                    bRecording = false;
        FVulkanRenderContext*   RenderContext = nullptr;
        FCommandListInfo        Info;
    };
}
