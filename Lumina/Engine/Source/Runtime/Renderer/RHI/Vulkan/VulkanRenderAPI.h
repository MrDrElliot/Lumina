#pragma once

#include "Assets/AssetHandle.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderContext.h"
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
        
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor) override;
        virtual void EndRender() override;
        virtual void WaitDevice() override;

        FRenderConfig GetConfig() override { return Config; }
        TRefPtr<FSwapchain> GetSwapchain() override;
        TRefPtr<FImage> GetSwapchainImage() override;
        ERHIInterfaceType GetRHIInterfaceType() override;

        void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo) override;
        void BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets) override;
        void BindPipeline(TRefPtr<FPipeline> Pipeline) override;
        
        void CopyToSwapchain(TRefPtr<FImage> ImageToCopy) override;
        void ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value) override;

        
        static std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout InLayout, uint32 InCount);
        static void FreeDescriptorSets(std::vector<VkDescriptorSet> InSets);

        void PushConstants(EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data) override;

        void RenderMeshTasks(TRefPtr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data) override;
        void RenderMeshIndexed(TRefPtr<FPipeline> Pipeline, TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer, FMiscData Data) override;
        void RenderVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance) override;
        void RenderStaticMeshWithMaterial(const TRefPtr<FPipeline>& Pipeline, const TSharedPtr<LStaticMesh>& StaticMesh, const TRefPtr<FMaterial>& Material) override;
        void RenderStaticMesh(const TRefPtr<FPipeline>& Pipeline, TSharedPtr<LStaticMesh> StaticMesh, uint32 InstanceCount) override;
        
        
        TRefPtr<FCommandBuffer> GetCommandBuffer() override;
        virtual void BeginCommandRecord() override;
        virtual void EndCommandRecord() override;
        virtual void ExecuteCurrentCommands() override;
    

    private:

        FRenderConfig Config;
        
        TSharedPtr<FVulkanRenderContext> RenderContext;
        TRefPtr<FVulkanSwapchain> Swapchain;

        TVector<TRefPtr<FVulkanCommandBuffer>> CommandBuffers;
        TRefPtr<FVulkanCommandBuffer> CurrentCommandBuffer;
        TRefPtr<FVulkanPipeline> CurrentBoundPipeline;
        
        static VkDescriptorPool DescriptorPool;
        
    };
}
