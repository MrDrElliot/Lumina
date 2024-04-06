#include "VulkanHelpers.h"

#include "VulkanRenderContext.h"
#include "Source/Runtime/ApplicationCore/Application.h"

namespace Lumina
{
    vkb::Instance Vulkan::GetInstance()
    {
        return FApplication::Get().GetRenderContext<FVulkanRenderContext>()->GetInstance();
    }

    VkCommandPoolCreateInfo Vulkan::CommandPoolCreateInfo(uint32_t InQueueFamilyIndex, VkCommandPoolCreateFlags InFlags)
    {
        VkCommandPoolCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        Info.pNext = nullptr;
        Info.queueFamilyIndex = InQueueFamilyIndex;
        Info.flags = InFlags;

        return Info;
    }

    VkCommandBufferAllocateInfo Vulkan::CommandBufferAllocateInfo(VkCommandPool InPool, uint32_t InCount)
    {
        VkCommandBufferAllocateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        Info.pNext = nullptr;
        Info.commandPool = InPool;
        Info.commandBufferCount = InCount;
        Info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    }

    VkFenceCreateInfo Vulkan::FenceCreateInfo(VkFenceCreateFlags InFlags)
    {
        VkFenceCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        Info.pNext = nullptr;
        Info.flags = InFlags;

        return Info;
    }

    VkSemaphoreCreateInfo Vulkan::SemaphoreCreateInfo(VkSemaphoreCreateFlags InFlags)
    {
        VkSemaphoreCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        Info.pNext = nullptr;
        Info.flags = InFlags;

        return Info;
    }
}