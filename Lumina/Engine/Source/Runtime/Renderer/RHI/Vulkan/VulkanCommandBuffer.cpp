#include "VulkanCommandBuffer.h"

#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"

namespace Lumina
{
    FVulkanCommandBuffer::FVulkanCommandBuffer()
    : CommandBuffer(nullptr), CommandPool(nullptr), Level(), BufferType(), CmdType()
    {
        std::unreachable();
    }

    FVulkanCommandBuffer::FVulkanCommandBuffer(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType)
    : CommandBuffer(VK_NULL_HANDLE), CommandPool(VK_NULL_HANDLE), Level(InLevel), BufferType(InBufferType), CmdType(InCmdType)
    {
        FQueueFamilyIndex Index = FRenderContext::GetQueueFamilyIndex();
        VkDevice Device = FVulkanRenderContext::GetDevice();
        
        VkCommandPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        PoolInfo.queueFamilyIndex = Index.Graphics;
        PoolInfo.flags = InBufferType == ECommandBufferType::GENERAL ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;

        vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool);

        VkCommandBufferAllocateInfo BufferInfo =  {};
        BufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        BufferInfo.commandPool = CommandPool;
        BufferInfo.commandBufferCount = 1;
        BufferInfo.level = Level == ECommandBufferLevel::PRIMARY ?  VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        vkAllocateCommandBuffers(Device, &BufferInfo, &CommandBuffer);
        
    }

    FVulkanCommandBuffer::~FVulkanCommandBuffer()
    {
    }

    void FVulkanCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        Info.flags = BufferType == ECommandBufferType::TRANSIENT ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;

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

    void FVulkanCommandBuffer::Execute(bool bWait)
    {
        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;

        VkQueue Queue = FVulkanRenderContext::GetGeneralQueue();

        VkFence Fence = VK_NULL_HANDLE;
        if(bWait)
        {
            VkFenceCreateInfo FenceInfo = {};
            FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

            vkCreateFence(FVulkanRenderContext::GetDevice(), &FenceInfo, nullptr, &Fence);
        }
        
        SubmissionMutex.lock();
        vkQueueSubmit(Queue, 1, &SubmitInfo, Fence);
        SubmissionMutex.unlock();

        if(bWait)
        {
            auto Device = FVulkanRenderContext::GetDevice();
            vkWaitForFences(Device, 1, &Fence, VK_TRUE, UINT64_MAX);
            vkDestroyFence(Device, Fence, nullptr);
        }
    }

    void FVulkanCommandBuffer::Destroy()
    {
        vkDestroyCommandPool(FVulkanRenderContext::GetDevice(), CommandPool, nullptr);
    }
}
