#include "VulkanHelpers.h"

#include <fstream>

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

        return Info;
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

    VkSemaphoreSubmitInfo Vulkan::SemaphoreSubmitInfo(VkPipelineStageFlags2 InStageMask, VkSemaphore InSemaphore)
    {
        VkSemaphoreSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.semaphore = InSemaphore;
        submitInfo.stageMask = InStageMask;
        submitInfo.deviceIndex = 0;
        submitInfo.value = 1;

        return submitInfo;
    }

    VkCommandBufferBeginInfo Vulkan::CommandBufferBeginInfo(VkCommandBufferUsageFlags InFlags)
    {
        VkCommandBufferBeginInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        Info.pNext = nullptr;
        Info.pInheritanceInfo = nullptr;
        Info.flags = InFlags;

        return Info;
    }

    VkCommandBufferSubmitInfo Vulkan::CommandBufferSubmitInfo(VkCommandBuffer InBuffer)
    {
        VkCommandBufferSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        info.pNext = nullptr;
        info.commandBuffer = InBuffer;
        info.deviceMask = 0;

        return info;
    }

    void Vulkan::TransitionImage(VkCommandBuffer InCmd, VkImage InImage, VkImageLayout CurrentLayout, VkImageLayout NewLayout)
    {
        VkImageMemoryBarrier2 ImageBarrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        ImageBarrier.pNext = nullptr;
        ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        ImageBarrier.oldLayout = CurrentLayout;
        ImageBarrier.newLayout = NewLayout;

        VkImageAspectFlags aspectMask = (NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        ImageBarrier.subresourceRange = ImageSubresourceRange(aspectMask);
        ImageBarrier.image = InImage;

        VkDependencyInfo depInfo {};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.pNext = nullptr;

        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &ImageBarrier;

        vkCmdPipelineBarrier2(InCmd, &depInfo);
    }

    VkSubmitInfo2 Vulkan::SubmitInfo(VkCommandBufferSubmitInfo* InCmd, VkSemaphoreSubmitInfo* SignalSemaphoreInfo, VkSemaphoreSubmitInfo* WaitSemaphoreInfo)
    {
        VkSubmitInfo2 info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info.pNext = nullptr;

        info.waitSemaphoreInfoCount = WaitSemaphoreInfo == nullptr ? 0 : 1;
        info.pWaitSemaphoreInfos = WaitSemaphoreInfo;

        info.signalSemaphoreInfoCount = SignalSemaphoreInfo == nullptr ? 0 : 1;
        info.pSignalSemaphoreInfos = SignalSemaphoreInfo;

        info.commandBufferInfoCount = 1;
        info.pCommandBufferInfos = InCmd;

        return info;
    }

    VkImageSubresourceRange Vulkan::ImageSubresourceRange(VkImageAspectFlags InAspectMask)
    {
        VkImageSubresourceRange Info = {};
        Info.aspectMask = InAspectMask;
        Info.baseMipLevel = 0;
        Info.levelCount = VK_REMAINING_MIP_LEVELS;
        Info.baseArrayLayer = 0;
        Info.layerCount = VK_REMAINING_ARRAY_LAYERS;

        return Info;
    }

    VkImageCreateInfo Vulkan::ImageCreateInfo(VkFormat InFormat, VkImageUsageFlags InUsageFlags, VkExtent3D InExtent)
    {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;

        info.imageType = VK_IMAGE_TYPE_2D;

        info.format = InFormat;
        info.extent = InExtent;

        info.mipLevels = 1;
        info.arrayLayers = 1;

        //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
        info.samples = VK_SAMPLE_COUNT_1_BIT;

        //optimal tiling, which means the image is stored on the best gpu format
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = InUsageFlags;

        return info;
    }

    VkImageViewCreateInfo Vulkan::ImageViewCreateInfo(VkFormat InFormat, VkImage InImage, VkImageAspectFlags AspectFlags)
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;

        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.image = InImage;
        info.format = InFormat;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.aspectMask = AspectFlags;

        return info;
    }

    void Vulkan::CopyImageToImage(VkCommandBuffer InCmd, VkImage Source, VkImage Destination, VkExtent2D SourceSize, VkExtent2D DestinationSize)
    {
        VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

        blitRegion.srcOffsets[1].x = SourceSize.width;
        blitRegion.srcOffsets[1].y = SourceSize.height;
        blitRegion.srcOffsets[1].z = 1;

        blitRegion.dstOffsets[1].x = DestinationSize.width;
        blitRegion.dstOffsets[1].y = DestinationSize.height;
        blitRegion.dstOffsets[1].z = 1;

        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcSubresource.mipLevel = 0;

        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstSubresource.mipLevel = 0;

        VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
        blitInfo.dstImage = Destination;
        blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blitInfo.srcImage = Source;
        blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitInfo.filter = VK_FILTER_LINEAR;
        blitInfo.regionCount = 1;
        blitInfo.pRegions = &blitRegion;

        vkCmdBlitImage2(InCmd, &blitInfo);
    }

    VkShaderModule* Vulkan::LoadShaderModule(const char* InFilePath, VkDevice InDevice)
    {
        std::ifstream File(InFilePath, std::ios::ate | std::ios::binary);

        if (!File.is_open())
        {
            LE_LOG_CRITICAL("Failed to open file");
            return nullptr;
        }


        size_t fileSize = (size_t)File.tellg();


        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

        File.seekg(0);

        File.read((char*)buffer.data(), fileSize);

        File.close();

        VkShaderModuleCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.pNext = nullptr;


        CreateInfo.codeSize = buffer.size() * sizeof(uint32_t);
        CreateInfo.pCode = buffer.data();

        VkShaderModule* shaderModule;
        if (vkCreateShaderModule(InDevice, &CreateInfo, nullptr, &*shaderModule) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to load shader module.");
            return nullptr;
        }
        return shaderModule;
    }
}
