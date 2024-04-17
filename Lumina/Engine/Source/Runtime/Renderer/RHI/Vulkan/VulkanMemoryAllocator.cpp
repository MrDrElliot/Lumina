#include "VulkanMemoryAllocator.h"

#include "VulkanRenderContext.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    FVulkanMemoryAllocator* FVulkanMemoryAllocator::Instance = nullptr;
    
    FVulkanMemoryAllocator::FVulkanMemoryAllocator()
    {
        FVulkanRenderContext& RenderContext = FVulkanRenderContext::Get();

        VmaVulkanFunctions Functions = {};
        Functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        Functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo Info = {};
        Info.vulkanApiVersion = VK_API_VERSION_1_3;
        Info.instance = RenderContext.GetVulkanInstance();
        Info.physicalDevice = RenderContext.GetPhysicalDevice();
        Info.device = RenderContext.GetDevice();
        Info.pVulkanFunctions = &Functions;

        vmaCreateAllocator(&Info, &Allocator);

        Statistics = { 0, 0, 0 };
    }

    FVulkanMemoryAllocator::~FVulkanMemoryAllocator()
    {
        vmaDestroyAllocator(Allocator);
    }

    void FVulkanMemoryAllocator::Init()
    {
        Instance = new FVulkanMemoryAllocator;
    }

    void FVulkanMemoryAllocator::Destroy()
    {
        delete Instance;
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateBuffer(VkBufferCreateInfo* CreateInfo, uint32_t Flags, VkBuffer* Buffer)
    {
        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;

        vmaCreateBuffer(Allocator, CreateInfo, &Info, Buffer, &Allocation, &AllocationInfo);

        Statistics.Allocated += AllocationInfo.size;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        
        return Allocation;
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateImage(VkImageCreateInfo* CreateInfo, uint32_t Flags, VkImage* Image)
    {
        if (CreateInfo->extent.depth == 0)
        {
            LE_LOG_WARN("Trying to allocate image with 0 depth. No allocation done");
        }
        
        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;

        vmaCreateImage(Allocator, CreateInfo, &Info, Image, &Allocation, &AllocationInfo);
        Statistics.Allocated += AllocationInfo.size;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        
        return Allocation;
    }

    void FVulkanMemoryAllocator::DestroyBuffer(VkBuffer* Buffer, VmaAllocation* Allocation)
    {
        VmaAllocationInfo AllocationInfo;
        vmaGetAllocationInfo(Allocator, *Allocation, &AllocationInfo);

        vmaDestroyBuffer(Allocator, *Buffer, *Allocation);

        Statistics.Freed += AllocationInfo.size;
        Statistics.CurrentlyAllocated -= AllocationInfo.size;

        *Buffer = VK_NULL_HANDLE;
        *Allocation = VK_NULL_HANDLE;
    }

    void FVulkanMemoryAllocator::DestroyImage(VkImage* Image, VmaAllocation* Allocation)
    {
        VmaAllocationInfo AllocationInfo;
        vmaGetAllocationInfo(Allocator, *Allocation, &AllocationInfo);

        vmaDestroyImage(Allocator, *Image, *Allocation);

        Statistics.Freed += AllocationInfo.size;
        Statistics.CurrentlyAllocated -= AllocationInfo.size;
        
        *Image = VK_NULL_HANDLE;
        *Allocation = VK_NULL_HANDLE;
    }

    void* FVulkanMemoryAllocator::MapMemory(VmaAllocation Allocation)
    {
        void* MappedMemory;
        vmaMapMemory(Allocator, Allocation, &MappedMemory);
        return MappedMemory;
    }

    void FVulkanMemoryAllocator::UnmapMemory(VmaAllocation Allocation)
    {
        vmaUnmapMemory(Allocator, Allocation);
    }
}
