#include "VulkanMemoryAllocator.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Renderer/RHIIncl.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    FVulkanMemoryAllocator::FVulkanMemoryAllocator()
    {
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
    		
        VmaVulkanFunctions Functions = {};
        Functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        Functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo Info = {};
        Info.vulkanApiVersion = VK_API_VERSION_1_3;
        Info.instance = RenderContext->GetVulkanInstance();
        Info.physicalDevice = RenderContext->GetPhysicalDevice();
        Info.device = RenderContext->GetDevice();
        Info.pVulkanFunctions = &Functions;

        VK_CHECK(vmaCreateAllocator(&Info, &Allocator));

        Statistics = {};
    }

    FVulkanMemoryAllocator::~FVulkanMemoryAllocator()
    {
    }

    void FVulkanMemoryAllocator::ClearAllAllocations()
    {
        // Cleanup allocated buffers
        for (auto& kvp : Statistics.AllocatedBuffers)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Buffer Object with VkBuffer: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyBuffer(Allocator, kvp.first, kvp.second);
            }
        }
        Statistics.AllocatedBuffers.clear(); // Ensure buffers map is empty after cleanup

        // Cleanup allocated images
        for (auto& kvp : Statistics.AllocatedImages)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Image Object with VkImage: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyImage(Allocator, kvp.first, kvp.second);
            }
        }
        
        Statistics.AllocatedImages.clear(); // Ensure images map is empty after cleanup

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

    VmaAllocation FVulkanMemoryAllocator::AllocateBuffer(VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName)
    {
        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation = nullptr;
        VmaAllocationInfo AllocationInfo;

        VK_CHECK(vmaCreateBuffer(Allocator, CreateInfo, &Info, vkBuffer, &Allocation, &AllocationInfo));
        AssertMsg(Allocation, "Vulkan failed to allocate buffer memory!");

        vmaSetAllocationName(Allocator, Allocation, AllocationName);

        Statistics.Allocations.push_back(Allocation);
        Statistics.AllocatedBuffers[*vkBuffer]= Allocation;
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

        Statistics.Allocated += AllocationInfo.size;
        Statistics.AllocatedImages[*vkImage] = Allocation;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        Statistics.CurrentlyAllocatedImages++;
        
        return Allocation;
    }



    void FVulkanMemoryAllocator::DestroyBuffer(VkBuffer Buffer, VmaAllocation Allocation)
    {
        if (Buffer && Allocation)
        {
            FRenderer::WaitIdle();
            
            VmaAllocationInfo AllocationInfo;
            vmaGetAllocationInfo(Allocator, Allocation, &AllocationInfo);

            LOG_WARN("Destroying Buffer: {0}", AllocationInfo.pName);

            Statistics.CurrentlyAllocated -= AllocationInfo.size;
            Statistics.CurrentlyAllocatedBuffers--;


            vmaDestroyBuffer(Allocator, Buffer, Allocation);
            
            Statistics.AllocatedBuffers.erase(Buffer);
            
            Allocation = VK_NULL_HANDLE;
        }
    }


    void FVulkanMemoryAllocator::DestroyImage(VkImage Image, VmaAllocation Allocation)
    {
        if (Image && Allocation)
        {
            FRenderer::WaitIdle();

            VmaAllocationInfo AllocationInfo;
            vmaGetAllocationInfo(Allocator, Allocation, &AllocationInfo);

            LOG_DEBUG("Destroying Image: {0}", AllocationInfo.pName);

            Statistics.CurrentlyAllocated -= AllocationInfo.size;
            Statistics.CurrentlyAllocatedImages--;
            
            vmaDestroyImage(Allocator, Image, Allocation);
            
            Statistics.AllocatedImages.erase(Image);

            Allocation = VK_NULL_HANDLE;
        }
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
