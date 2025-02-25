#pragma once

#include <vulkan/vulkan.h>

#include "VulkanMemoryAllocator.h"
#include "Renderer/Buffer.h"

namespace Lumina
{

    struct FIndexBufferData
    {
        uint64 IndexCount;
        VkIndexType IndexType;
    };

    struct FVertexBufferData
    {
        uint64 VertexCount;
    };


    constexpr VkIndexType ExtractIndexType(uint8 Flags)
    {
        if (Flags & (uint64)EDeviceBufferFlags::INDEX_TYPE_UINT8)		return VK_INDEX_TYPE_UINT8_EXT;
        if (Flags & (uint64)EDeviceBufferFlags::INDEX_TYPE_UINT16)	    return VK_INDEX_TYPE_UINT16;
        if (Flags & (uint64)EDeviceBufferFlags::INDEX_TYPE_UINT32)	    return VK_INDEX_TYPE_UINT32;
        return {};
    }

    
    class FVulkanBuffer : public FBuffer
    {
    public:

        FVulkanBuffer(const FDeviceBufferSpecification& Spec);
        ~FVulkanBuffer() override;

        void SetFriendlyName(const FString& InName) override;
        
        VkBuffer& GetBuffer() { return Buffer; }
        VkBuffer* GetBufferPtr() { return &Buffer; }
        VmaAllocation GetAllocation() const { return Allocation; }

        FDeviceBufferSpecification GetSpecification() const override;

        void UploadData(uint64 Offset, void* Data, uint64 DataSize) override;
        void Resize() override { std::unreachable(); }
    
    private:

        uint64 GetAlignedSizeForBuffer(uint64 Size, EDeviceBufferUsage Usage);

    private:
    
        VkBuffer Buffer;
        VmaAllocation Allocation;

    };
}
