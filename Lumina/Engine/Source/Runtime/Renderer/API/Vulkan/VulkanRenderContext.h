#pragma once
#include "TrackedCommandBuffer.h"
#include "Core/Threading/Thread.h"

#ifdef LUMINA_RENDERER_VULKAN

#include "VulkanMacros.h"
#include "Types/BitFlags.h"
#include "src/VkBootstrap.h"
#include "Renderer/RenderContext.h"
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>


namespace Lumina
{
    class FVulkanCommandList;
    class FVulkanSwapchain;
    class FVulkanDevice;
    enum class ECommandQueue : uint8;
}

namespace Lumina
{
    class FFencePool : public IDeviceChild
    {
    public:
        
        FFencePool(FVulkanDevice* InDevice)
            : IDeviceChild(InDevice)
        {
        }

        ~FFencePool()
        {
            for (VkFence Fence : Fences)
            {
                vkDestroyFence(Device->GetDevice(), Fence, nullptr);
            }
        }

        NODISCARD VkFence Aquire()
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
            VK_CHECK(vkCreateFence(Device->GetDevice(), &FenceCreateInfo, nullptr, &Fence));
            return Fence;
        }

        void Release(VkFence Fence)
        {
            vkResetFences(Device->GetDevice(), 1, &Fence);
            Fences.push_back(Fence);
        }

    private:

        TVector<VkFence> Fences;
    };
    
    class FQueue : public IDeviceChild
    {
    public:

        FQueue(FVulkanDevice* InDevice, VkQueue InQueue, uint32 InQueueFamilyIndex)
            : IDeviceChild(InDevice)
            , CommandPool(nullptr)
            , Queue(InQueue)
            , QueueFamilyIndex(InQueueFamilyIndex)
            , FencePool(InDevice)
        {}


        TRefCountPtr<FTrackedCommandBufer> GetOrCreateCommandBuffer();

        void RetireCommandBuffers();
        
        void Submit(ICommandList* CommandLists, uint32 NumCommandLists);
        
        
        void AddSignalSemaphore(VkSemaphore Semaphore);
        void AddWaitSemaphore(VkSemaphore Semaphore);

        
        FMutex                      Mutex;
        VkCommandPool               CommandPool;
        VkQueue                     Queue;
        uint32                      QueueFamilyIndex;
        TVector<VkSemaphore>        WaitSemaphores;
        TVector<VkSemaphore>        SignalSemaphores;
        FFencePool                  FencePool;
        
        TVector<TRefCountPtr<FTrackedCommandBufer>> CommandBuffersInFlight;
        TStack<TRefCountPtr<FTrackedCommandBufer>>  CommandBufferPool;
    };
    
    class FVulkanRenderContext : public IRenderContext
    {
    public:
        
        FVulkanRenderContext();
        
        void Initialize() override;
        void Deinitialize() override;

        void SetVSyncEnabled(bool bEnable) override;
        bool IsVSyncEnabled() const override;

        void WaitIdle() override;
        
        void FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) override;
        void FrameEnd(const FUpdateContext& UpdateContext) override;

        FORCEINLINE FQueue* GetQueue(ECommandQueue Type) const { return Queues[(uint32)Type]; }

        FRHICommandListRef CreateCommandList(const FCommandListInfo& Info) override;
        void ExecuteCommandList(ICommandList* CommandLists, uint32 NumCommandLists, ECommandQueue QueueType) override;
        FRHICommandListRef GetCommandList(ECommandQueue Queue) override;
        
        void CreateDevice(vkb::Instance Instance);

        FORCEINLINE VkInstance GetVulkanInstance() const { return VulkanInstance; }
        FORCEINLINE FVulkanDevice* GetDevice() const { return VulkanDevice; }
        FORCEINLINE FVulkanSwapchain* GetSwapchain() const { return Swapchain; }
        
        //----------------------------------------------------


        NODISCARD FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Description) override;
        uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage) override;

        
        //-------------------------------------------------------------------------------------
        
        

        NODISCARD FRHIImageRef CreateImage(const FRHIImageDesc& ImageSpec) override;
        
        
        //-------------------------------------------------------------------------------------


        NODISCARD FRHIVertexShaderRef CreateVertexShader(const TVector<uint32>& ByteCode) override;
        NODISCARD FRHIPixelShaderRef CreatePixelShader(const TVector<uint32>& ByteCode) override;
        NODISCARD FRHIComputeShaderRef CreateComputeShader(const TVector<uint32>& ByteCode) override;


        //-------------------------------------------------------------------------------------
        


        void FlushPendingDeletes() override;
        
        
        void SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle);

    
    private:
        
        uint8                                           CurrentFrameIndex;
        TArray<FQueue*, (uint32)ECommandQueue::Num>     Queues;
        FRHICommandListRef                              CommandList = nullptr;
         
        VkInstance                                      VulkanInstance;
        
        FVulkanSwapchain*                               Swapchain = nullptr;
        FVulkanDevice*                                  VulkanDevice = nullptr;
        
    };
    
}
#endif