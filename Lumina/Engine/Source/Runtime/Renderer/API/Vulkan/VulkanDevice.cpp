#include "VulkanDevice.h"

#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "VulkanResources.h"
#include "Core/Profiler/Profile.h"

namespace Lumina
{

    FVulkanMemoryAllocator::FVulkanMemoryAllocator(FVulkanRenderContext* InCxt, VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkDevice Device)
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

        RenderContext = InCxt;

    }
    
    
    FVulkanMemoryAllocator::~FVulkanMemoryAllocator()
    {
        ClearAllAllocations();
    }

    void FVulkanMemoryAllocator::ClearAllAllocations()
    {
        LUMINA_PROFILE_SCOPE();

        for (auto& kvp : AllocatedBuffers)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Buffer Object with VkBuffer: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyBuffer(Allocator, kvp.first, kvp.second);
            }
        }
        
        AllocatedBuffers.clear();

        for (auto& kvp : AllocatedImages)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Image Object with VkImage: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyImage(Allocator, kvp.first, kvp.second);
            }
        }
        
        AllocatedImages.clear();
        
        vmaDestroyAllocator(Allocator);
        Allocator = VK_NULL_HANDLE;
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateBuffer(const VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName)
    {
        LUMINA_PROFILE_SCOPE();
        FScopeLock Lock(BufferAllocationMutex);

        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation = nullptr;
        VmaAllocationInfo AllocationInfo;

        VK_CHECK(vmaCreateBuffer(Allocator, CreateInfo, &Info, vkBuffer, &Allocation, &AllocationInfo));
        AssertMsg(Allocation, "Vulkan failed to allocate buffer memory!");

#if LE_DEBUG
        vmaSetAllocationName(Allocator, Allocation, AllocationName);
#endif
        
        AllocatedBuffers[*vkBuffer]= Allocation;
        
        return Allocation;
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName)
    {
        LUMINA_PROFILE_SCOPE();
        FScopeLock Lock(ImageAllocationMutex);

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

        
#if LE_DEBUG
        if (AllocationName && strlen(AllocationName) > 0)
        {
            vmaSetAllocationName(Allocator, Allocation, AllocationName);
        }
#endif
        
        AllocatedImages[*vkImage] = Allocation;
        
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
        LUMINA_PROFILE_SCOPE();
        FScopeLock Lock(BufferAllocationMutex);

        Assert(Buffer != VK_NULL_HANDLE)

        VmaAllocationInfo AllocationInfo;
        vmaGetAllocationInfo(Allocator, AllocatedBuffers[Buffer], &AllocationInfo);
        
        auto it = AllocatedBuffers.find(Buffer);
        if (it == AllocatedBuffers.end())
        {
            LOG_CRITICAL("Buffer was not found in VulkanMemoryAllocator!");
            return;
        }
        
        
        vmaDestroyBuffer(Allocator, Buffer, AllocatedBuffers[Buffer]);
        
        AllocatedBuffers.erase(Buffer);
        
    }


    void FVulkanMemoryAllocator::DestroyImage(VkImage Image)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(Image)

        FScopeLock Lock(ImageAllocationMutex);
        
        VmaAllocationInfo AllocationInfo;
        vmaGetAllocationInfo(Allocator, AllocatedImages[Image], &AllocationInfo);
        
        vmaDestroyImage(Allocator, Image, AllocatedImages[Image]);
        
        AllocatedImages.erase(Image);

    }

    void* FVulkanMemoryAllocator::MapMemory(FVulkanBuffer* Buffer, VmaAllocation Allocation)
    {
        LUMINA_PROFILE_SCOPE();

        // If the buffer has been used in a command list before, wait for that CL to complete
        if (Buffer->LastUseCommandListID != 0)
        {
            FQueue* Queue = RenderContext->GetQueue(Buffer->LastUseQueue);
            Queue->WaitCommandList(Buffer->LastUseCommandListID, UINT64_MAX);
        }
        
        void* MappedMemory;
        VK_CHECK(vmaMapMemory(Allocator, Allocation, &MappedMemory));
        return MappedMemory;
    }

    void FVulkanMemoryAllocator::UnmapMemory(FVulkanBuffer* Buffer, VmaAllocation Allocation)
    {
        LUMINA_PROFILE_SCOPE();
        vmaUnmapMemory(Allocator, Allocation);
    }
}
