#pragma once
#include "vk_mem_alloc.h"
#include <glm/glm.hpp>

namespace Lumina
{
    
    
    class FVulkanMemoryAllocator
    {
    public:

        FVulkanMemoryAllocator();
        ~FVulkanMemoryAllocator();

        static void Init();
        static void Destroy();
        static FVulkanMemoryAllocator* Get() { return Instance; }

        VmaAllocation AllocateBuffer(VkBufferCreateInfo* CreateInfo, uint32_t Flags, VkBuffer* Buffer);
        VmaAllocation AllocateImage(VkImageCreateInfo* CreateInfo, uint32_t Flags, VkImage* Image);

        void DestroyBuffer(VkBuffer* Buffer, VmaAllocation* Allocation);
        void DestroyImage(VkImage* Image, VmaAllocation* Allocation);

        void* MapMemory(VmaAllocation Allocation);
        void UnmapMemory(VmaAllocation Allocation);



    private:

        static FVulkanMemoryAllocator* Instance;

        VmaAllocator Allocator;

        struct AllocatorStatistics 
        {
            glm::uint64 Allocated;
            glm::uint64 Freed;
            glm::uint64 CurrentlyAllocated;
        } Statistics;

    };
}
