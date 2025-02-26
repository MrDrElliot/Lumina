#pragma once
#include "VulkanTypes.h"

#ifdef LUMINA_RENDERER_VULKAN

#include "Types/BitFlags.h"
#include "src/VkBootstrap.h"
#include "Renderer/RenderHandle.h"
#include "Renderer/RenderContext.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>


namespace Lumina
{
    class FVulkanSwapchain;
    enum class ECommandQueue : uint8;
}

namespace Lumina
{

    class FVulkanMemoryAllocator
    {
    public:

        FVulkanMemoryAllocator(VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkDevice Device);
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

    struct FVulkanCommandList : public FCommandList
    {
        
        VkCommandBuffer CommandBuffer;
        
    };
    
    struct FVulkanCommandQueues
    {
        VkQueue     GraphicsQueue;
        uint32      GraphicsQueueIndex;
        
        VkQueue     TransferQueue;
        uint32      TransferQueueIndex;
        
        VkQueue     ComputeQueue;
        uint32      ComputeQueueIndex;
        
    };
    
    class FVulkanRenderContext : public IRenderContext
    {
    public:

        using FBufferPool = TRenderResourcePool<FRHIBufferHandle, FVulkanBuffer>;
        using FImagePool = TRenderResourcePool<FRHIImageHandle, FVulkanImage>;
        
        FVulkanRenderContext();
        void Initialize() override;
        void Deinitialize() override;

        void CreateDevice(vkb::Instance Instance);

        VkDevice GetDevice() const { return Device; }
        VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
        FORCEINLINE FVulkanSwapchain* GetSwapchain() const { return Swapchain; }

        FORCEINLINE const FVulkanCommandQueues& GetCommandQueues() const { return CommandQueues; }

        FORCEINLINE const FBufferPool& GetBufferPool() const { return BufferPool; }
        FORCEINLINE const FImagePool& GetImagePool() const { return ImagePool; }
        
        //----------------------------------------------------

        
        FRHIBufferHandle CreateBuffer(TBitFlags<ERenderDeviceBufferUsage> UsageFlags, TBitFlags<ERenderDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) override;
        void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset) override;
        void CopyBuffer(FRHIBufferHandle Source, FRHIBufferHandle Destination) override;
        uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<ERenderDeviceBufferUsage> Usage) override;

        FRHIImageHandle CreateTexture(FVector2D Extent) override;
        FRHIImageHandle CreateRenderTarget(FVector2D Extent) override;
        FRHIImageHandle CreateDepthImage(FVector2D Extent) override;

        void Barrier(FGPUBarrier* Barriers, uint32 BarrierNum, FCommandList* CommandList) override;
        
        //-------------------------------------------------------------------------------------

        FCommandList* BeginCommandList(ECommandQueue CommandType, ECommandBufferUsage Usage) override;
        void EndCommandList(FCommandList* CommandList, bool bDestroy) override;
        
        void BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass(FCommandList* CommandList) override;
        
    private:

        TVector<FVulkanCommandList*>            CommandList;
        FBufferPool                             BufferPool;
        FImagePool                              ImagePool;

        FVulkanSwapchain*                       Swapchain;
        VkInstance                              VulkanInstance;
        
        FVulkanCommandQueues                    CommandQueues;
        VkDevice                                Device;
        VkPhysicalDevice                        PhysicalDevice;
        VkPhysicalDeviceProperties              PhysicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties        PhysicalDeviceMemoryProperties;
        FVulkanMemoryAllocator*                 MemoryAllocator = nullptr;
    };
}
#endif