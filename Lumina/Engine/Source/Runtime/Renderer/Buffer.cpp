#include "Buffer.h"

#include "Core/Assertions/Assert.h"
#include "RHI/Vulkan/VulkanBuffer.h"

namespace Lumina
{
    TRefPtr<FBuffer> FBuffer::Create(const FDeviceBufferSpecification& Spec)
    {
        return MakeRefPtr<FVulkanBuffer>(Spec);
    }

    TRefPtr<FBuffer> FBuffer::Create(const FDeviceBufferSpecification& Spec, void* Data, uint64 DataSize)
    {
        AssertMsg(Data, "Cannot create a buffer with empty data!");
        return MakeRefPtr<FVulkanBuffer>(Spec, Data, DataSize);
    }
}
