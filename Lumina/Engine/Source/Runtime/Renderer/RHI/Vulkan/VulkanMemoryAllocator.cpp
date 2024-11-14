#include "VulkanMemoryAllocator.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Memory/Memory.h"
#include "Source/Runtime/Log/Log.h"
#include "vulkan/vulkan.h"

namespace Lumina
{
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

        VK_CHECK(vmaCreateAllocator(&Info, &Allocator));

        Statistics = {};
    }

    FVulkanMemoryAllocator::~FVulkanMemoryAllocator()
    {
    }

    void FVulkanMemoryAllocator::Destroy()
    {
        if (Get()->Allocator)
        {
            for(auto& kvp : Get()->Statistics.AllocatedBuffers)
            {
                if(kvp.first != VK_NULL_HANDLE)
                {
                    LOG_DEBUG("Destroying Buffer Object!");
                    //kvp.first->Destroy();
                    vmaDestroyBuffer(Get()->Allocator, kvp.first, kvp.second);
                }
            }
            
            for(auto& kvp : Get()->Statistics.AllocatedImages)
            {
                if(kvp.first != VK_NULL_HANDLE)
                {
                    LOG_DEBUG("Destroying Image Object!");
                    //vkDestroyImageView(FVulkanRenderContext::GetDevice(), kvp.first->GetImageView(), nullptr);
                    vmaDestroyImage(Get()->Allocator, kvp.first, kvp.second);
                }
            }
            
            
            vmaDestroyAllocator(Get()->Allocator);
            Get()->Allocator = VK_NULL_HANDLE;
        }
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateBuffer(VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName)
    {
        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation;
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
            VmaAllocationInfo AllocationInfo;
            vmaGetAllocationInfo(Allocator, Allocation, &AllocationInfo);

            LOG_WARN("Destroying Image: {0}", AllocationInfo.pName);

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
