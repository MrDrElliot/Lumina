#pragma once

#include <vma/vk_mem_alloc.h>
#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Platform/GenericPlatform.h"



namespace Lumina
{
    class FVulkanRenderContext;
    class FVulkanImage;
    class FVulkanBuffer;
    
    class FVulkanMemoryAllocator : public TSingleton<FVulkanMemoryAllocator>
    {
    public:

        FVulkanMemoryAllocator();
        ~FVulkanMemoryAllocator();

        void ClearAllAllocations();
        
        VmaAllocation AllocateBuffer(VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName);
        VmaAllocation AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName);

        void DestroyBuffer(VkBuffer Buffer, VmaAllocation Allocation);
        void DestroyImage(VkImage Image, VmaAllocation Allocation);

        void* MapMemory(VmaAllocation Allocation);
        void UnmapMemory(VmaAllocation Allocation);

    
    private:
        
        VmaAllocator Allocator = nullptr;

        struct FAllocatorStatistics 
        {
            TVector<VmaAllocation> Allocations;
            THashMap<VkBuffer, VmaAllocation> AllocatedBuffers;
            THashMap<VkImage, VmaAllocation> AllocatedImages;
            uint64 Allocated = 0;
            uint64 Freed = 0;
            uint64 CurrentlyAllocated = 0;
            uint64 CurrentlyAllocatedBuffers = 0;
            uint64 CurrentlyAllocatedImages = 0;
        } Statistics;

    };
}
