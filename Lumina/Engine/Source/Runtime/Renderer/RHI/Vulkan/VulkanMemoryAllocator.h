#pragma once

#include <EASTL/unordered_map.h>
#include "vk_mem_alloc.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "Core/Singleton/Singleton.h"
#include "Platform/GenericPlatform.h"



namespace Lumina
{
    class FVulkanImage;
    class FVulkanBuffer;
    class FVulkanMemoryAllocator : public TSingleton<FVulkanMemoryAllocator>
    {
    public:

        FVulkanMemoryAllocator();
        ~FVulkanMemoryAllocator();
        
        VmaAllocation AllocateBuffer(VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName);
        VmaAllocation AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName);

        void DestroyBuffer(VkBuffer Buffer, VmaAllocation Allocation);
        void DestroyImage(VkImage Image, VmaAllocation Allocation);

        void* MapMemory(VmaAllocation Allocation);
        void UnmapMemory(VmaAllocation Allocation);

    
    private:
        
        VmaAllocator Allocator = nullptr;

        struct AllocatorStatistics 
        {
            std::vector<VmaAllocation> Allocations;
            eastl::unordered_map<VkBuffer, VmaAllocation> AllocatedBuffers;
            eastl::unordered_map<VkImage, VmaAllocation> AllocatedImages;
            uint64 Allocated = 0;
            uint64 Freed = 0;
            uint64 CurrentlyAllocated = 0;
            uint64 CurrentlyAllocatedBuffers = 0;
            uint64 CurrentlyAllocatedImages = 0;
        } Statistics;

    };
}
