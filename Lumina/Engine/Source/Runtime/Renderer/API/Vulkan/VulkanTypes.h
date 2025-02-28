#pragma once

#ifdef LUMINA_RENDERER_VULKAN
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace Lumina
{
    enum class EShaderDataType : uint8;
}

namespace Lumina
{
    
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