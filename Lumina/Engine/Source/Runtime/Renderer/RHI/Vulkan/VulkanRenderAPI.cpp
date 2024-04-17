#include "VulkanRenderAPI.h"

#include "VulkanImage.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    VkDescriptorPool FVulkanRenderAPI::DescriptorPool = VK_NULL_HANDLE;
    
    FVulkanRenderAPI::FVulkanRenderAPI(const FRenderConfig& InConfig)
    {
        LE_LOG_INFO("Vulkan Render API: Initializing");

        Config = InConfig;

        RenderContext = std::make_shared<FVulkanRenderContext>(InConfig);
        Swapchain = RenderContext->GetSwapchain();

        CommandBuffers.resize(Swapchain->GetSpecs().FramesInFlight);
        CommandBuffers.shrink_to_fit();

        for (auto& Buffer : CommandBuffers)
        {
            Buffer = std::make_shared<FVulkanCommandBuffer>(false);
        }

        if (DescriptorPool == VK_NULL_HANDLE)
        {
            glm::uint32 Count = UINT16_MAX * InConfig.FramesInFlight;

            std::vector<VkDescriptorPoolSize> PoolSizes =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER,					    Count }, 
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	    Count },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				Count },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				Count },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			    Count },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			    Count },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			Count }
            };

            VkDescriptorPoolCreateInfo PoolInfo = {};
            PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            PoolInfo.maxSets = 1000;
            PoolInfo.poolSizeCount = PoolSizes.size();
            PoolInfo.pPoolSizes = PoolSizes.data();
            PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            vkCreateDescriptorPool(RenderContext->GetDevice(), &PoolInfo, nullptr, &DescriptorPool);

        }
    }

    FVulkanRenderAPI::~FVulkanRenderAPI()
    {
        LE_LOG_WARN("Vulkan Render API: Shutting Down");

        vkDeviceWaitIdle(FVulkanRenderContext::GetDevice());

        for(auto& Buffer : CommandBuffers)
        {
            Buffer->Destroy();
        }

        vkDestroyDescriptorPool(FVulkanRenderContext::GetDevice(), DescriptorPool, nullptr);

    }

    void FVulkanRenderAPI::BeginFrame()
    {
        Swapchain->BeginFrame();
        CurrentCommandBuffer = CommandBuffers[Swapchain->GetCurrentFrameIndex()];
        BeginCommandRecord();
    }

    void FVulkanRenderAPI::EndFrame()
    {
        Swapchain->EndFrame();
    }

    void FVulkanRenderAPI::BeginRender(std::shared_ptr<FImage> Target, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor)
    {
    }

    void FVulkanRenderAPI::EndRender()
    {
    }

    void FVulkanRenderAPI::WaitDevice()
    {
        vkDeviceWaitIdle(RenderContext->GetDevice());
    }

    std::shared_ptr<FImage> FVulkanRenderAPI::GetSwapchainImage()
    {
        return Swapchain->GetCurrentImage();
    }

    void FVulkanRenderAPI::ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value)
    {

        std::shared_ptr<FVulkanImage> VkImage = std::dynamic_pointer_cast<FVulkanImage>(Image);
        
        VkClearColorValue ClearColorValue = {};
        ClearColorValue.float32[0] = Value.r;
        ClearColorValue.float32[1] = Value.g;
        ClearColorValue.float32[2] = Value.b;
        ClearColorValue.float32[3] = Value.a;

        VkImageSubresourceRange Range = {};
        Range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Range.baseMipLevel = 0;
        Range.levelCount = 1;
        Range.baseArrayLayer = 0;
        Range.layerCount = 1;

        vkCmdClearColorImage(CurrentCommandBuffer->GetCommandBuffer(),  VkImage->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue, 1, &Range);
    }

    std::shared_ptr<FCommandBuffer> FVulkanRenderAPI::GetCommandBuffer()
    {
        return CurrentCommandBuffer;
    }

    void FVulkanRenderAPI::BeginCommandRecord()
    {
        FRenderer::Submit([&](std::shared_ptr<FCommandBuffer> CmdBuffer)
        {
            CmdBuffer->Reset();
            CmdBuffer->Begin();
        });
    }

    void FVulkanRenderAPI::EndCommandRecord()
    {
        FRenderer::Submit([&](std::shared_ptr<FCommandBuffer> CmdBuffer)
        {
           CmdBuffer->End(); 
        });
    }

    void FVulkanRenderAPI::ExecuteCurrentCommands()
    {
        FRenderer::Submit([&](std::shared_ptr<FCommandBuffer> CmdBuffer)
        {
            VkPipelineStageFlags StageMasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            auto VkCmdBuffer = std::dynamic_pointer_cast<FVulkanCommandBuffer>(CmdBuffer);
            auto Semaphores = Swapchain->GetSemaphores();

            VkSubmitInfo SubmitInfo = {};
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = &VkCmdBuffer->GetCommandBuffer();
            SubmitInfo.signalSemaphoreCount = 1;
            SubmitInfo.pSignalSemaphores = &Semaphores.Render;
            SubmitInfo.waitSemaphoreCount = 1;
            SubmitInfo.pWaitSemaphores = &Semaphores.Present;
            SubmitInfo.pWaitDstStageMask = StageMasks;

            FVulkanRenderContext& Context = FVulkanRenderContext::Get();
            
            vkQueueSubmit(Context.GetGeneralQueue(), 1, &SubmitInfo, Swapchain->GetCurrentFence());
            
        });
    }
}
