#pragma once
#include "vk_mem_alloc.h"
#include <glm/glm.hpp>

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Memory/RefCounted.h"
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
        void Shutdown() override;
        
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
            std::unordered_map<VkBuffer, VmaAllocation> AllocatedBuffers;
            std::unordered_map<VkImage, VmaAllocation> AllocatedImages;
            uint64 Allocated = 0;
            uint64 Freed = 0;
            uint64 CurrentlyAllocated = 0;
            uint64 CurrentlyAllocatedBuffers = 0;
            uint64 CurrentlyAllocatedImages = 0;
        } Statistics;

    };
}
