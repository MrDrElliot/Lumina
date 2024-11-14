#include "CommandBuffer.h"

#include "RHI/Vulkan/VulkanCommandBuffer.h"

namespace Lumina
{
    TRefPtr<FCommandBuffer> FCommandBuffer::Create(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType)
    {
        return MakeRefPtr<FVulkanCommandBuffer>(InLevel, InBufferType, InCmdType);
    }
}
