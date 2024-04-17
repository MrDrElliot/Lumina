#include "VulkanCommandBuffer.h"

#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"

namespace Lumina
{
    FVulkanCommandBuffer::FVulkanCommandBuffer()
    {
    }

    FVulkanCommandBuffer::FVulkanCommandBuffer(bool bTransient): CommandBuffer(VK_NULL_HANDLE), CommandPool(VK_NULL_HANDLE)
    {
        FQueueFamilyIndex Index = FVulkanRenderContext::GetQueueFamilyIndex();
        VkDevice Device = FVulkanRenderContext::GetDevice();
        
        VkCommandPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        PoolInfo.queueFamilyIndex = Index.Graphics;
        PoolInfo.flags = bTransient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;

        vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool);

        VkCommandBufferAllocateInfo BufferInfo =  {};
        BufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        BufferInfo.commandPool = CommandPool;
        BufferInfo.commandBufferCount = 1;
        BufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        vkAllocateCommandBuffers(Device, &BufferInfo, &CommandBuffer);
        
    }

    FVulkanCommandBuffer::~FVulkanCommandBuffer()
    {
    }

    void FVulkanCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        Info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(CommandBuffer, &Info);
    }

    void FVulkanCommandBuffer::End()
    {
        vkEndCommandBuffer(CommandBuffer);
    }

    void FVulkanCommandBuffer::Reset()
    {
        VkDevice Device = FVulkanRenderContext::GetDevice();
        vkResetCommandPool(Device, CommandPool, 0);
    }

    void FVulkanCommandBuffer::Execute()
    {
        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;

        VkQueue Queue = FVulkanRenderContext::GetGeneralQueue();

        VkFence Fence = VK_NULL_HANDLE;
        VkFenceCreateInfo FenceInfo = {};
        FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        vkCreateFence(FVulkanRenderContext::GetDevice(), &FenceInfo, nullptr, &Fence);
        
        vkQueueSubmit(Queue, 1, &SubmitInfo, Fence);
    }

    void FVulkanCommandBuffer::Destroy()
    {
        vkDestroyCommandPool(FVulkanRenderContext::GetDevice(), CommandPool, nullptr);
    }
}
