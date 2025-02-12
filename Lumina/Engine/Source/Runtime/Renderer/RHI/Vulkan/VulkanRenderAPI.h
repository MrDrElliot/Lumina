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

        FVulkanRenderAPI(const FRenderConfig& InConfig);
        ~FVulkanRenderAPI() override;
        ERHIInterfaceType GetRHIInterfaceType() override;
        void Initialize(const FRenderConfig& InConfig) override;
        
        
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void BeginRender(const TVector<TRefPtr<FImage>>& Attachments, const glm::fvec4& ClearColor) override;
        virtual void EndRender() override;
        virtual void WaitDevice() override;
        
        void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo) override;
        void BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets) override;
        void BindPipeline(TRefPtr<FPipeline> Pipeline) override;
        
        void CopyToSwapchain(TRefPtr<FImage> ImageToCopy) override;
        void ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value) override;

        void PushConstants(TRefPtr<FPipeline> Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data) override;

        void DrawIndexed(TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer) override;
        void DrawVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance) override;
        
        
        virtual void BeginCommandRecord() override;
        virtual void EndCommandRecord() override;
        virtual void ExecuteCurrentCommands() override;
    

    private:
        
    };
}
