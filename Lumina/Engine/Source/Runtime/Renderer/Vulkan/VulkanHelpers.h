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

    /* Creates a VkSemaphoreSubmit Info */
    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 InStageMask, VkSemaphore InSemaphore);

    /* Creates a VkCommandBufferBeginInfo */
    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags InFlags = 0);

    /* Creates a VkCommabdBufferSubmitInfo */
    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer InBuffer);

    /* Transition the swap chain image into a drawable layout, then clear the command */
    void TransitionImage(VkCommandBuffer InCmd, VkImage InImage, VkImageLayout CurrentLayout, VkImageLayout NewLayout);

    /* Creates a VkSubmitInfo2 */
    VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* InCmd, VkSemaphoreSubmitInfo* SignalSemaphoreInfo, VkSemaphoreSubmitInfo* WaitSemaphoreInfo);

    /* Creates a VkImageSubresourceRange */
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags InAspectMask);

    /* Creates a VkRenderPassBeginInfo */
    VkRenderPassBeginInfo RenderpassBeginInfo(VkRenderPass InRenderPass, VkExtent2D InExtent, VkFramebuffer InBuffer);

    /* Creates a VkImageCreateInfo */
    VkImageCreateInfo ImageCreateInfo(VkFormat InFormat, VkImageUsageFlags InUsageFlags, VkExtent3D InExtent);

    /* Creates a VkImageViewCreateInfo */
    VkImageViewCreateInfo ImageViewCreateInfo(VkFormat InFormat, VkImage InImage, VkImageAspectFlags AspectFlags);

    /* Copies an image to another image */
    void CopyImageToImage(VkCommandBuffer InCmd, VkImage Source, VkImage Destination, VkExtent2D SourceSize, VkExtent2D DestinationSize);

    /* Loads a shader module */
    VkShaderModule* LoadShaderModule(const char* InFilePath, VkDevice InDevice);
    
}
