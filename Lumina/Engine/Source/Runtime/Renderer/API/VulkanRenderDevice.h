#pragma once

#ifdef LUMINA_RENDERER_VULKAN

#include "Types/BitFlags.h"
#include "src/VkBootstrap.h"
#include "Renderer/RenderDevice.h"
#include <vulkan/vulkan.hpp>


namespace Lumina
{
    class FVulkanMemoryAllocator;
}

namespace Lumina
{

    struct FVulkanCommandQueues
    {
        VkQueue     GraphicsQueue;
        uint32      GraphicsQueueIndex;
        
        VkQueue     PresentQueue;
        uint32      PresentQueueIndex;
        
        VkQueue     TransferQueue;
        uint32      TransferQueueIndex;
        
        VkQueue     ComputeQueue;
        uint32      ComputeQueueIndex;
        
    };
    
    class FVulkanRenderDevice : public IRenderDevice
    {
    public:

        FVulkanRenderDevice();
        void Initialize() override;

        void CreateDevice(vkb::Instance Instance);

        void SetPhysicalDevice(VkPhysicalDevice Device);
        void SetLogicalDevice(VkDevice Device);

        //----------------------------------------------------

        FRHICommandBufferHandle CreateCommandBuffer() override;
        FRHIBufferHandle CreateBuffer(TBitFlags<EDeviceBufferUsage> UsageFlags, TBitFlags<EDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) override;
        void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset) override;
    
    private:

        uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EDeviceBufferUsage> Usage);

    private:
        
        FVulkanCommandQueues                CommandQueues;
        VkDevice                            LogicalDevice;
        VkPhysicalDevice                    PhysicalDevice;
        VkPhysicalDeviceProperties          PhysicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties    PhysicalDeviceMemoryProperties;
        FVulkanMemoryAllocator*             MemoryAllocator = nullptr;
    };
}
#endif