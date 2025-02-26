#pragma once

#ifdef LUMINA_RENDERER_VULKAN

#include "VulkanMacros.h"
#include "VulkanTypes.h"
#include "Memory/Allocators/Allocator.h"
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
    class FFencePool
    {
    public:
        FFencePool() = default;

        void SetDevice(VkDevice InDevice)
        {
            Device = InDevice;
        }
            
        VkFence Aquire()
        {
            if (!Fences.empty())
            {
                VkFence Fence = Fences.back();
                Fences.pop_back();
                return Fence;
            }

            VkFence Fence;
            VkFenceCreateInfo FenceCreateInfo = {};
            FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            VK_CHECK(vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence));
            return Fence;
        }

        void Release(VkFence Fence)
        {
            vkResetFences(Device, 1, &Fence);
            Fences.push_back(Fence);
        }

        void Destroy()
        {
            for (VkFence Fence : Fences)
            {
                vkDestroyFence(Device, Fence, nullptr);
            }
        }

    private:
        
        TVector<VkFence> Fences;
        VkDevice         Device;
    };
    
    
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

    struct FVulkanCommandList : FCommandList
    {
        
        VkCommandBuffer CommandBuffer;
        VkFence Fence;
        
        TVector<VkSemaphore> WaitSemaphores;
        TVector<VkSemaphore> SignalSemaphores;
        
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

        void SetVSyncEnabled(bool bEnable) override;
        bool IsVSyncEnabled() const override;

        void WaitIdle() override;
        
        void FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) override;
        void FrameEnd(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) override;
        

        void CreateDevice(vkb::Instance Instance);

        VkInstance GetVulkanInstance() const { return VulkanInstance; }
        VkDevice GetDevice() const { return Device; }
        VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
        FORCEINLINE FVulkanSwapchain* GetSwapchain() const { return Swapchain; }

        FORCEINLINE const FVulkanCommandQueues& GetCommandQueues() const { return CommandQueues; }

        FORCEINLINE FBufferPool& GetBufferPool() { return BufferPool; }
        FORCEINLINE FImagePool& GetImagePool() { return ImagePool; }
        
        //----------------------------------------------------

        
        FRHIBufferHandle CreateBuffer(TBitFlags<ERenderDeviceBufferUsage> UsageFlags, TBitFlags<ERenderDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) override;
        void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset) override;
        void CopyBuffer(FRHIBufferHandle Source, FRHIBufferHandle Destination) override;
        uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<ERenderDeviceBufferUsage> Usage) override;

        FRHIImageHandle AllocateImage() override;
        FRHIImageHandle CreateTexture(const FImageSpecification& ImageSpec) override;
        FRHIImageHandle CreateRenderTarget(const FIntVector2D& Extent) override;
        FRHIImageHandle CreateDepthImage(const FImageSpecification& ImageSpec) override;

        void Barrier(FGPUBarrier* Barriers, uint32 BarrierNum, FCommandList* CommandList) override;
        
        //-------------------------------------------------------------------------------------

        FVulkanCommandList* GetPrimaryCommandList() const;
        FCommandList* BeginCommandList(ECommandBufferLevel Level = ECommandBufferLevel::Secondary, ECommandQueue CommandType = ECommandQueue::Graphics, ECommandBufferUsage Usage = ECommandBufferUsage::General) override;
        void EndCommandList(FCommandList* CommandList) override;

        
        //-------------------------------------------------------------------------------------

        
        void BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass(FCommandList* CommandList) override;

        void ClearColor(FCommandList* CommandList, const FColor& Color) override;
        

        //-------------------------------------------------------------------------------------

        
        
        void SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle);

    
    private:
        
        uint8                                   CurrentFrameIndex;
        FVulkanCommandList*                     PrimaryCommandList[FRAMES_IN_FLIGHT];
        TQueue<FVulkanCommandList*>             CommandQueue;
        
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
        FFencePool                              FencePool;
        
        
    };
    
}
#endif