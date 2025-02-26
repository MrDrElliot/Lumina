#pragma once

#ifdef LUMINA_RENDERER_VULKAN

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace Lumina
{
    struct FVulkanBuffer
    {
        uint64 Size;
        VkBuffer Buffer;
        VmaAllocation Allocation;
    };

    struct FVulkanImage
    {
        VkImageUsageFlags UsageFlags;
        VkImageLayout Layout;
        VkImage Image;
        VmaAllocation Allocation;
        VkImageView ImageView;
    };
    
}

#endif