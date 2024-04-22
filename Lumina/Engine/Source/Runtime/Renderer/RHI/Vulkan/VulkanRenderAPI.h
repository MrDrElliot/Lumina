#pragma once
#include "VulkanCommandBuffer.h"
#include "VulkanRenderContext.h"
#include "VulkanSwapchain.h"
#include "Source/Runtime/Renderer/RenderAPI.h"
#include "Source/Runtime/Renderer/Renderer.h"



namespace Lumina
{
    
    class FVulkanRenderAPI : public FRenderAPI
    {
    public:

        FVulkanRenderAPI(const FRenderConfig& InConfig);
        ~FVulkanRenderAPI() override;
        
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void BeginRender(const std::vector<std::shared_ptr<FImage>> Attachments, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor) override;
        virtual void EndRender() override;
        virtual void WaitDevice() override;

        std::shared_ptr<FSwapchain> GetSwapchain() override;
        std::shared_ptr<FImage> GetSwapchainImage() override;

        void BindSet(std::shared_ptr<FDescriptorSet> Set, std::shared_ptr<FPipeline> Pipeline, glm::uint8 Index) override;
        void BindPipeline(std::shared_ptr<FPipeline> Pipeline) override;
        
        void CopyToSwapchain(std::shared_ptr<FImage> ImageToCopy) override;
        void ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value) override;

        
        static std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout InLayout, glm::uint32 InCount);
        static void FreeDescriptorSets(std::vector<VkDescriptorSet> InSets);

        void RenderMeshTasks(std::shared_ptr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data) override;
        void RenderMeshIndexed(std::shared_ptr<FBuffer> VertexBuffer, std::shared_ptr<FBuffer> IndexBuffer) override;
        void RenderQuad(std::shared_ptr<FPipeline> Pipeline, FMiscData Data) override;
        void RenderQuad(std::shared_ptr<FPipeline> Pipeline, glm::uint32 Amount, FMiscData Data) override;
        
        
        std::shared_ptr<FCommandBuffer> GetCommandBuffer() override;
        virtual void BeginCommandRecord() override;
        virtual void EndCommandRecord() override;
        virtual void ExecuteCurrentCommands() override;

        void RenderImGui() override;


    private:

        FRenderConfig Config;

        std::shared_ptr<FVulkanSwapchain> Swapchain;
        std::shared_ptr<FVulkanRenderContext> RenderContext;

        std::vector<std::shared_ptr<FVulkanCommandBuffer>> CommandBuffers;
        std::shared_ptr<FVulkanCommandBuffer> CurrentCommandBuffer;

        static VkDescriptorPool DescriptorPool;
        
    };
}
