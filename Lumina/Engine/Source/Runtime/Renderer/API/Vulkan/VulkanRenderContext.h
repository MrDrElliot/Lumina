#pragma once

#ifdef LUMINA_RENDERER_VULKAN

#include <vulkan/vulkan.hpp>
#include "VulkanMacros.h"
#include "Types/BitFlags.h"
#include "src/VkBootstrap.h"
#include "Renderer/RenderContext.h"
#include <vma/vk_mem_alloc.h>


namespace Lumina
{
    class FVulkanCommandList;
    class FVulkanSwapchain;
    class FVulkanDevice;
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
        
        FVulkanRenderContext();
        
        void Initialize() override;
        void Deinitialize() override;

        void SetVSyncEnabled(bool bEnable) override;
        bool IsVSyncEnabled() const override;

        void WaitIdle() override;
        
        void FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) override;
        void FrameEnd(const FUpdateContext& UpdateContext) override;
        

        void CreateDevice(vkb::Instance Instance);

        VkInstance GetVulkanInstance() const { return VulkanInstance; }
        FVulkanDevice* GetDevice() const { return VulkanDevice; }
        FORCEINLINE FVulkanSwapchain* GetSwapchain() const { return Swapchain; }

        FORCEINLINE const FVulkanCommandQueues& GetCommandQueues() const { return CommandQueues; }
        
        //----------------------------------------------------


        FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Description) override;
        void UploadToBuffer(ICommandList* CommandList, FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size) override;
        void CopyBuffer(ICommandList* CommandList, FRHIBuffer* Source, FRHIBuffer* Destination) override;
        uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage) override;

        
        //-------------------------------------------------------------------------------------
        

        

        FRHIImageRef CreateImage(const FRHIImageDesc& ImageSpec) override;
        
        
        //-------------------------------------------------------------------------------------

        

        NODISCARD FVulkanCommandList* GetPrimaryCommandList() const;
        NODISCARD ICommandList* AllocateCommandList(ECommandBufferLevel Level = ECommandBufferLevel::Secondary, ECommandQueue CommandType = ECommandQueue::Graphics, ECommandBufferUsage Usage = ECommandBufferUsage::General) override;

        
        //-------------------------------------------------------------------------------------

        
        void BeginRenderPass(ICommandList* CommandList, const FRenderPassBeginInfo& PassInfo) override;
        void EndRenderPass(ICommandList* CommandList) override;

        void ClearColor(ICommandList* CommandList, const FColor& Color) override;

        void Draw(ICommandList* CommandList, uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) override;
        void DrawIndexed(ICommandList* CommandList, uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance) override;
        void Dispatch(ICommandList* CommandList, uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) override;

        //-------------------------------------------------------------------------------------

        void FlushPendingDeletes() override;
        
        FFencePool* GetFencePool() { return &FencePool; }
        FVulkanCommandQueues& GetCommandQueues() { return CommandQueues; }
        
        
        void SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle);

    
    private:
        
        uint8                                   CurrentFrameIndex;
        FVulkanCommandList*                     PrimaryCommandList[FRAMES_IN_FLIGHT];
        TQueue<FVulkanCommandList*>             CommandQueue;
        
        FVulkanSwapchain*                       Swapchain;
        VkInstance                              VulkanInstance;
        
        FVulkanCommandQueues                    CommandQueues;
        
        FVulkanDevice*                          VulkanDevice;
        
        FFencePool                              FencePool;
        
    };
    
}
#endif