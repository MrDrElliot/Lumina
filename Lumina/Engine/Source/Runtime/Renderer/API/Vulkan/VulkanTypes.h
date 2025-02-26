#pragma once

#ifdef LUMINA_RENDERER_VULKAN

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace Lumina
{
    enum class EShaderDataType : uint8;
}

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
        VkImageLayout CurrentLayout;
        VkImageLayout DefaultLayout;
        VkImage Image;
        VmaAllocation Allocation;
        VkImageView ImageView;
    };

    struct FVulkanGraphicsPipeline
    {
        VkPipeline Pipeline;
    };

    struct FVulkanComputePipeline
    {
        VkPipeline Pipeline;
    };

    struct FVulkanShader
    {
        TVector<uint32> ShaderBinaries;
        VkShaderStageFlags StageFlags;
        VkShaderModule Module;
    };
    
}

#endif