#include "VulkanCommandList.h"

#include "VulkanDevice.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Renderer/RenderContext.h"

namespace Lumina
{
    void FVulkanCommandList::Begin()
    {
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = (Type == ECommandBufferUsage::Transient) ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_NO_FLAGS;
        BeginInfo.pInheritanceInfo = nullptr;

        Fence = RenderContext->GetFencePool()->Aquire();
        WaitSemaphores.clear();
        SignalSemaphores.clear();
        
        VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &BeginInfo));
    }

    void FVulkanCommandList::FlushCommandList()
    {
        VK_CHECK(vkEndCommandBuffer(CommandBuffer));
    }

    void FVulkanCommandList::SubmitCommandList()
    {
        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;
        SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
        SubmitInfo.waitSemaphoreCount = WaitSemaphores.size();
        SubmitInfo.pSignalSemaphores = SignalSemaphores.data();
        SubmitInfo.signalSemaphoreCount = SignalSemaphores.size();

        if (!WaitSemaphores.empty())
        {
            VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            SubmitInfo.pWaitDstStageMask = WaitStages;
        }
        
        VK_CHECK(vkQueueSubmit(Queue, 1, &SubmitInfo, Fence));
        VK_CHECK(vkWaitForFences(RenderContext->GetDevice()->GetDevice(), 1, &Fence, VK_TRUE, UINT64_MAX));
    }

    void FVulkanCommandList::Reset()
    {
        VK_CHECK(vkResetCommandBuffer(CommandBuffer, 0));
        RenderContext->GetFencePool()->Release(Fence);
    }

    void FVulkanCommandList::Destroy()
    {
        RenderContext->GetFencePool()->Release(Fence);
        vkFreeCommandBuffers(RenderContext->GetDevice()->GetDevice(), Pool, 1, &CommandBuffer);
        FVulkanCommandList* MutableThis = this;
        FMemory::Delete(MutableThis);
    }
}
