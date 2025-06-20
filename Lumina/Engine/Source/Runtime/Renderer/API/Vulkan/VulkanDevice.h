#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "Containers/Array.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class FVulkanRenderContext;
}

namespace Lumina
{

    class FVulkanMemoryAllocator
    {
    public:

        FVulkanMemoryAllocator(VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkDevice Device);
        ~FVulkanMemoryAllocator();

        void ClearAllAllocations();
        
        VmaAllocation AllocateBuffer(const VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName);
        VmaAllocation AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName);

        VmaAllocation GetAllocation(VkBuffer Buffer);
        VmaAllocation GetAllocation(VkImage Image);
        

        void DestroyBuffer(VkBuffer Buffer);
        void DestroyImage(VkImage Image);

        void* MapMemory(VmaAllocation Allocation);
        void UnmapMemory(VmaAllocation Allocation);

    
    private:
        
        VmaAllocator Allocator = nullptr;
        THashMap<VkBuffer, VmaAllocation> AllocatedBuffers;
        THashMap<VkImage, VmaAllocation> AllocatedImages;

    };
    
    class FVulkanDevice
    {
    public:

        FVulkanDevice(VkInstance Instance, VkPhysicalDevice InPhysicalDevice, VkDevice InDevice)
            : PhysicalDevice(InPhysicalDevice)
            , Device(InDevice)
        {
            vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
            vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

            Allocator = Memory::New<FVulkanMemoryAllocator>(Instance, PhysicalDevice, Device);
        }

        ~FVulkanDevice()
        {
            Memory::Delete(Allocator);
            vkDestroyDevice(Device, nullptr);
        }

        

        FORCEINLINE FVulkanMemoryAllocator* GetAllocator() const { return Allocator; }
        FORCEINLINE VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
        FORCEINLINE VkDevice GetDevice() const { return Device; }

        FORCEINLINE VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return PhysicalDeviceProperties; }
        FORCEINLINE VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return PhysicalDeviceMemoryProperties; }
    
    private:

        FVulkanMemoryAllocator* Allocator = nullptr;
        VkPhysicalDevice        PhysicalDevice;
        VkDevice                Device;

        VkPhysicalDeviceProperties              PhysicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties        PhysicalDeviceMemoryProperties;
    };

    class IDeviceChild
    {
    protected:
        
        ~IDeviceChild() = default;

    public:
        
        IDeviceChild(FVulkanDevice* InDevice)
            :Device(InDevice)
        {}

        
        FVulkanDevice* Device = nullptr;
        
    };
    
}
