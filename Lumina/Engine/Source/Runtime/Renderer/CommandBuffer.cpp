#include "CommandBuffer.h"

#include "RHI/Vulkan/VulkanCommandBuffer.h"

namespace Lumina
{
    std::shared_ptr<FCommandBuffer> FCommandBuffer::Create(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType)
    {
        return std::make_shared<FVulkanCommandBuffer>(InLevel, InBufferType, InCmdType);
    }
}
