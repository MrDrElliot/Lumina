#include "Buffer.h"

#include "Renderer/RHIIncl.h"
#include "Core/Assertions/Assert.h"
#include "RHI/Vulkan/VulkanBuffer.h"

namespace Lumina
{
    FRHIBuffer FBuffer::Create(const FDeviceBufferSpecification& Spec)
    {
        return FRHIBuffer(MakeRefCount<FVulkanBuffer>(Spec));
    }

    FRHIBuffer FBuffer::Create(const FDeviceBufferSpecification& Spec, void* Data, uint64 DataSize)
    {
        AssertMsg(Data, "Cannot create a buffer with empty data!");
        return FRHIBuffer(MakeRefCount<FVulkanBuffer>(Spec, Data, DataSize));
    }
}
