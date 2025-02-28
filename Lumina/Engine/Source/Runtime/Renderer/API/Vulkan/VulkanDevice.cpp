#include "VulkanDevice.h"

#include "VulkanMacros.h"

namespace Lumina
{

    FVulkanMemoryAllocator::FVulkanMemoryAllocator(VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkDevice Device)
    {
        VmaVulkanFunctions Functions = {};
        Functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        Functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo Info = {};
        Info.vulkanApiVersion = VK_API_VERSION_1_3;
        Info.instance = Instance;
        Info.physicalDevice = PhysicalDevice;
        Info.device = Device;
        Info.pVulkanFunctions = &Functions;

        VK_CHECK(vmaCreateAllocator(&Info, &Allocator));

        Statistics = {};
    }
    
    
    FVulkanMemoryAllocator::~FVulkanMemoryAllocator()
    {
        ClearAllAllocations();
    }

    void FVulkanMemoryAllocator::ClearAllAllocations()
    {
        // Cleanup allocated buffers
        for (auto& kvp : AllocatedBuffers)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Buffer Object with VkBuffer: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyBuffer(Allocator, kvp.first, kvp.second);
            }
        }
        
        AllocatedBuffers.clear();

        // Cleanup allocated images
        for (auto& kvp : AllocatedImages)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Image Object with VkImage: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyImage(Allocator, kvp.first, kvp.second);
            }
        }
        
        AllocatedImages.clear();

        // Destroy the Vulkan memory allocator
        LOG_INFO("Destroying Vulkan Memory Allocator...");
        vmaDestroyAllocator(Allocator);
        Allocator = VK_NULL_HANDLE;

        // Reset statistics
        Statistics.CurrentlyAllocatedBuffers = 0;
        Statistics.CurrentlyAllocatedImages = 0;
        Statistics.CurrentlyAllocated = 0;

        // Log final state
        LOG_INFO("Allocator Shutdown Complete. Final Statistics:");
        LOG_INFO("Allocated Buffers: {}", Statistics.CurrentlyAllocatedBuffers);
        LOG_INFO("Allocated Images: {}", Statistics.CurrentlyAllocatedImages);
        LOG_INFO("Currently Allocated Memory: {} bytes", Statistics.CurrentlyAllocated);
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateBuffer(const VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName)
    {
        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation = nullptr;
        VmaAllocationInfo AllocationInfo;

        VK_CHECK(vmaCreateBuffer(Allocator, CreateInfo, &Info, vkBuffer, &Allocation, &AllocationInfo));
        AssertMsg(Allocation, "Vulkan failed to allocate buffer memory!");

        vmaSetAllocationName(Allocator, Allocation, AllocationName);

        AllocatedBuffers[*vkBuffer]= Allocation;
        Statistics.Allocated += AllocationInfo.size;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        Statistics.CurrentlyAllocatedBuffers++;
        
        return Allocation;
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName)
    {
        if (CreateInfo->extent.depth == 0)
        {
            LOG_WARN("Trying to allocate image with 0 depth. No allocation done");
            return VK_NULL_HANDLE;
        }

        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;

        VK_CHECK(vmaCreateImage(Allocator, CreateInfo, &Info, vkImage, &Allocation, &AllocationInfo));
        AssertMsg(Allocation, "Vulkan failed to allocate image memory!");

        
        vmaSetAllocationName(Allocator, Allocation, AllocationName);

        AllocatedImages[*vkImage] = Allocation;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        Statistics.CurrentlyAllocatedImages++;
        
        return Allocation;
    }

    VmaAllocation FVulkanMemoryAllocator::GetAllocation(VkBuffer Buffer)
    {
        return AllocatedBuffers[Buffer];
    }

    VmaAllocation FVulkanMemoryAllocator::GetAllocation(VkImage Image)
    {
        return AllocatedImages[Image];
    }


    void FVulkanMemoryAllocator::DestroyBuffer(VkBuffer Buffer)
    {
        Assert(Buffer);

        VmaAllocationInfo AllocationInfo;
        vmaGetAllocationInfo(Allocator, AllocatedBuffers[Buffer], &AllocationInfo);
        
        Statistics.CurrentlyAllocated -= AllocationInfo.size;
        Statistics.CurrentlyAllocatedBuffers--;
        
        vmaDestroyBuffer(Allocator, Buffer, AllocatedBuffers[Buffer]);
        
        AllocatedBuffers.erase(Buffer);
        
    }


    void FVulkanMemoryAllocator::DestroyImage(VkImage Image)
    {
        Assert(Image);
        
        VmaAllocationInfo AllocationInfo;
        vmaGetAllocationInfo(Allocator, AllocatedImages[Image], &AllocationInfo);
        
        Statistics.CurrentlyAllocated -= AllocationInfo.size;
        Statistics.CurrentlyAllocatedImages--;
        
        vmaDestroyImage(Allocator, Image, AllocatedImages[Image]);
        
        AllocatedImages.erase(Image);

    }

    void* FVulkanMemoryAllocator::MapMemory(VmaAllocation Allocation)
    {
        void* MappedMemory;
        VK_CHECK(vmaMapMemory(Allocator, Allocation, &MappedMemory));
        return MappedMemory;
    }

    void FVulkanMemoryAllocator::UnmapMemory(VmaAllocation Allocation)
    {
        vmaUnmapMemory(Allocator, Allocation);
    }
}
