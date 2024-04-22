#pragma once
#include "Source/Runtime/Renderer/Buffer.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lumina
{

    struct FIndexBufferData
    {
        glm::uint32 IndexCount;
        VkIndexType IndexType;
    };

    struct FVertexBufferData
    {
        glm::uint32 VertexCount;
    };

    constexpr VkIndexType ExtractIndexType(glm::uint8 Flags)
    {
        if (Flags & (glm::uint64)EDeviceBufferFlags::INDEX_TYPE_UINT8)		return VK_INDEX_TYPE_UINT8_EXT;
        if (Flags & (glm::uint64)EDeviceBufferFlags::INDEX_TYPE_UINT16)	    return VK_INDEX_TYPE_UINT16;
        if (Flags & (glm::uint64)EDeviceBufferFlags::INDEX_TYPE_UINT32)	    return VK_INDEX_TYPE_UINT32;
        return {};
    }

    
    class FVulkanBuffer : public FBuffer
    {
    public:

        FVulkanBuffer(const FDeviceBufferSpecification& Spec);
        FVulkanBuffer(const FDeviceBufferSpecification& Spec, void* Data, glm::uint64 DataSize);

        VkBuffer GetBuffer() const { return Buffer; }
        VmaAllocation GetAllocation() const { return Allocation; }
        void* GetAdditionalData() const { return Data; }

        FDeviceBufferSpecification GetSpecification() const override;

        void UploadData(glm::uint64 Offset, void* Data, glm::uint64 DataSize) override;
        void Destroy() override;

    private:
    
        VkBuffer Buffer;
        VmaAllocation Allocation;

        FDeviceBufferSpecification Specification;
        void* Data;
    
    };
}
