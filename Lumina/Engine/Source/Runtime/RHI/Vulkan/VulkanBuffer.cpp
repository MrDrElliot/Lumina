#include "VulkanBuffer.h"

#include <cassert>


namespace Lumina
{
    VkResult FVulkanBuffer::Map(VkDeviceSize InSize, VkDeviceSize InOffset)
    {
        return vkMapMemory(Device, Memory, InOffset, InSize, 0, &Mapped);
    }

    void FVulkanBuffer::Unmap()
    {
        if(Mapped)
        {
            vkUnmapMemory(Device, Memory);
            Mapped = nullptr;
        }
    }

    VkResult FVulkanBuffer::Bind(VkDeviceSize InOffset)
    {
        return vkBindBufferMemory(Device, Buffer, Memory, InOffset);
    }

    void FVulkanBuffer::SetupDescriptor(VkDeviceSize InSize, VkDeviceSize InOffset)
    {
        Descriptor.offset = InOffset;
        Descriptor.buffer = Buffer;
        Descriptor.range = InSize;
    }

    void FVulkanBuffer::CopyTo(void* InData, VkDeviceSize InSize)
    {
        assert(Mapped);
        memcpy(Mapped, InData, InSize);
    }

    VkResult FVulkanBuffer::Flush(VkDeviceSize InSize, VkDeviceSize InOffset)
    {
        VkMappedMemoryRange MappedRange = {};
        MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        MappedRange.memory = Memory;
        MappedRange.offset = InOffset;
        MappedRange.size = InSize;

        return vkFlushMappedMemoryRanges(Device, 1, &MappedRange);
    }

    VkResult FVulkanBuffer::Invalidate(VkDeviceSize InSize, VkDeviceSize InOffset)
    {
        VkMappedMemoryRange MappedRange = {};
        MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        MappedRange.memory = Memory;
        MappedRange.offset = InOffset;
        MappedRange.size = InSize;

        return vkInvalidateMappedMemoryRanges(Device, 1, &MappedRange);
    }

    void FVulkanBuffer::Destroy()
    {
        if(Buffer)
        {
            vkDestroyBuffer(Device, Buffer, nullptr);
        }
        if(Memory)
        {
            vkFreeMemory(Device, Memory, nullptr);
        }
    }
}
