#pragma once

#include "Assets/AssetHandle.h"
#include "Renderer/RenderAPI.h"
#include "Renderer/Renderer.h"
#include "VulkanPipeline.h"

namespace Lumina
{
    class FVulkanRenderAPI : public IRenderAPI
    {
    public:

        FVulkanRenderAPI();
        ~FVulkanRenderAPI() override;
        ERHIInterfaceType GetRHIInterfaceType() override;
        void Initialize() override;
        
        
        virtual void BeginFrame() override;
        virtual void Present() override;
        virtual void BeginRender(const FRenderPassBeginInfo& Info) override;
        virtual void EndRender() override;
        virtual void WaitDevice() override;
        
        void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo) override;
        void BindSet(const FRHIDescriptorSet& Set, const TRefCountPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets) override;
        void BindPipeline(FRHIPipeline Pipeline) override;
        
        void CopyToSwapchain(FRHIImage ImageToCopy) override;
        void ClearColor(const FRHIImage& Image, const glm::fvec4& Value) override;

        void PushConstants(FRHIPipeline Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data) override;

        void SetShaderParameter(const FName& ParameterName, void* Data, uint32 Size) override;
        
        void BindVertexBuffer(FRHIBuffer VertexBuffer) override;
        void BindIndexBuffer(FRHIBuffer IndexBuffer) override;

        void DrawIndexed(uint32 IndexCount, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0) override;
        void DrawVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance) override;
        
        
        virtual void BeginCommandRecord() override;
        virtual void EndCommandRecord() override;
        virtual void ExecuteCurrentCommands() override;
    

    private:
        
    };
}
