#pragma once

#ifdef LUMINA_RENDERER_VULKAN


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
        VkImageUsageFlags Usage;
        VkImage Image;
        VmaAllocation Allocation;
        VkImageView ImageView;
    };
    
}

#endif