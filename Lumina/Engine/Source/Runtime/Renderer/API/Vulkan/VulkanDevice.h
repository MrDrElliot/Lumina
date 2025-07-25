﻿#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "Lumina.h"
#include "Containers/Array.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class FVulkanBuffer;
    class FRHIBuffer;
    class IDeviceChild;
    class FVulkanRenderContext;
}

namespace Lumina
{

    class FVulkanMemoryAllocator
    {
    public:

        FVulkanMemoryAllocator(FVulkanRenderContext* InCxt, VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkDevice Device);
        ~FVulkanMemoryAllocator();

        void ClearAllAllocations();
        
        VmaAllocation AllocateBuffer(const VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName);
        VmaAllocation AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName);

        VmaAllocation GetAllocation(VkBuffer Buffer);
        VmaAllocation GetAllocation(VkImage Image);

        VmaAllocator GetAllocator() const { return Allocator; }

        void DestroyBuffer(VkBuffer Buffer);
        void DestroyImage(VkImage Image);

        void* MapMemory(FVulkanBuffer* Buffer, VmaAllocation Allocation);
        void UnmapMemory(FVulkanBuffer* Buffer, VmaAllocation Allocation);

    
    private:

        FMutex ImageAllocationMutex;
        FMutex BufferAllocationMutex;
        VmaAllocator Allocator = nullptr;
        THashMap<VkBuffer, VmaAllocation> AllocatedBuffers;
        THashMap<VkImage, VmaAllocation> AllocatedImages;
        FVulkanRenderContext* RenderContext = nullptr;

    };
    
    class FVulkanDevice
    {
    public:

        FVulkanDevice(FVulkanRenderContext* RenderContext, VkInstance Instance, VkPhysicalDevice InPhysicalDevice, VkDevice InDevice)
            : PhysicalDevice(InPhysicalDevice)
            , Device(InDevice)
        {
            vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
            vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

            Allocator = Memory::New<FVulkanMemoryAllocator>(RenderContext, Instance, PhysicalDevice, Device);
        }

        virtual ~FVulkanDevice()
        {
            for (auto it = Children.rbegin(); it != Children.rend(); ++it)
            {
                Memory::Delete(*it);
            }

            Children.clear();
            Memory::Delete(Allocator);
            vkDestroyDevice(Device, nullptr);
        }

        void AddChild(IDeviceChild* InChild);
        void RemoveChild(IDeviceChild* InChild);

        FORCEINLINE FVulkanMemoryAllocator* GetAllocator() const { return Allocator; }
        FORCEINLINE VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
        FORCEINLINE VkDevice GetDevice() const { return Device; }

        FORCEINLINE VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return PhysicalDeviceProperties; }
        FORCEINLINE VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return PhysicalDeviceMemoryProperties; }
    
    private:

        FMutex                                  ChildMutex;
        FVulkanMemoryAllocator*                 Allocator = nullptr;
        VkPhysicalDevice                        PhysicalDevice;
        VkDevice                                Device;

        VkPhysicalDeviceProperties              PhysicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties        PhysicalDeviceMemoryProperties;
        TFixedVector<IDeviceChild*, 1000>       Children;
    };

    class IDeviceChild
    {
    public:

        IDeviceChild(FVulkanDevice* InDevice)
            :Device(InDevice)
        {
            Device->AddChild(this);
        }
        
        virtual ~IDeviceChild()
        {
            Device->RemoveChild(this);
        }
        
        SIZE_T         Index = INDEX_NONE;
        FVulkanDevice* Device = nullptr;
    };
    
}
