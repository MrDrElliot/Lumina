#include "CommandBuffer.h"

#include "RHI/Vulkan/VulkanCommandBuffer.h"

namespace Lumina
{
    std::shared_ptr<FCommandBuffer> FCommandBuffer::Create(bool bTransient)
    {
        return std::make_shared<FVulkanCommandBuffer>(bTransient);
    }
}
