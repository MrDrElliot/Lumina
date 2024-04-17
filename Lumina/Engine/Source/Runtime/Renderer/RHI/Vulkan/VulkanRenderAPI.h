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
        virtual void BeginRender(std::shared_ptr<FImage> Target, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor) override;
        virtual void EndRender() override;
        virtual void WaitDevice() override;

        std::shared_ptr<FImage> GetSwapchainImage() override;

        void ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value) override;

        std::shared_ptr<FCommandBuffer> GetCommandBuffer() override;
        virtual void BeginCommandRecord() override;
        virtual void EndCommandRecord() override;
        virtual void ExecuteCurrentCommands() override;


    private:

        FRenderConfig Config;

        std::shared_ptr<FVulkanSwapchain> Swapchain;
        std::shared_ptr<FVulkanRenderContext> RenderContext;

        std::vector<std::shared_ptr<FVulkanCommandBuffer>> CommandBuffers;
        std::shared_ptr<FVulkanCommandBuffer> CurrentCommandBuffer;

        static VkDescriptorPool DescriptorPool;
        
    };
}
