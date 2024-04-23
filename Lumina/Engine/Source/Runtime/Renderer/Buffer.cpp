#include "Buffer.h"

#include "RHI/Vulkan/VulkanBuffer.h"

namespace Lumina
{
    std::shared_ptr<FBuffer> FBuffer::Create(const FDeviceBufferSpecification& Spec)
    {
        return std::make_shared<FVulkanBuffer>(Spec);
    }

    std::shared_ptr<FBuffer> FBuffer::Create(const FDeviceBufferSpecification& Spec, void* Data, glm::uint64 DataSize)
    {
        return std::make_shared<FVulkanBuffer>(Spec, Data, DataSize);
    }
}
