#pragma once

#include <vulkan/vulkan_core.h>

#include "VkBootstrap.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina::Vulkan
{

#define VK_CHECK(x)                                                 \
    do                                                              \
    {                                                               \
        VkResult err = x;                                           \
        if (err)                                                    \
        {                                                           \
            LE_LOG_CRITICAL("Vulcan Error: {0}", err);              \
            abort();                                                \
        }                                                           \
} while (0)

    

    vkb::Instance GetInstance();


    /* Creates a VkCommandPoolCreateInfo */
    VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t InQueueFamilyIndex, VkCommandPoolCreateFlags InFlags = 0);

    /* Creates a VkCommandBufferAllocateInfo */
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool InPool, uint32_t InCount = 1);

    /* Creates a VkFenceCreateInfo */
    VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags InFlags = 0);

    /* Creates a VkSemaphoreCreateInfo */
    VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags InFlags = 0);


    
}
