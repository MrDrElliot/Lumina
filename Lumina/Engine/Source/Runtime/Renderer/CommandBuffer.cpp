#include "CommandBuffer.h"

#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanCommandBuffer.h"

namespace Lumina
{
    FRHICommandBuffer FCommandBuffer::Create(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType)
    {
        return FRHICommandBuffer(MakeRefCount<FVulkanCommandBuffer>(InLevel, InBufferType, InCmdType));
    }
}
