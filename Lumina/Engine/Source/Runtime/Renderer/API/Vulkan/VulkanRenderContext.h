#pragma once

#include "TrackedCommandBuffer.h"
#include "VulkanPipelineCache.h"
#include "Core/Threading/Thread.h"
#include "VulkanMacros.h"
#include "VulkanResources.h"
#include "Types/BitFlags.h"
#include "src/VkBootstrap.h"
#include "Renderer/RenderContext.h"
#include <tracy/TracyVulkan.hpp>

#include "VulkanDescriptorCache.h"

namespace Lumina
{
    class FSpirVShaderCompiler;
    class FVulkanCommandList;
    class FVulkanSwapchain;
    class FVulkanDevice;
    enum class ECommandQueue : uint8;
}

#define USE_VK_ALLOC_CALLBACK 1

namespace Lumina
{
    extern VkAllocationCallbacks GVulkanAllocationCallbacks;

#if USE_VK_ALLOC_CALLBACK
    #define VK_ALLOC_CALLBACK &GVulkanAllocationCallbacks
#else
    #define VK_ALLOC_CALLBACK nullptr
#endif
    
    struct FVulkanRenderContextFunctions
    {
        VkDebugUtilsMessengerEXT DebugMessenger = nullptr;
        PFN_vkSetDebugUtilsObjectNameEXT DebugUtilsObjectNameEXT = nullptr;
        PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT = nullptr;
        PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT = nullptr;
    };
    
    class FQueue : public IDeviceChild
    {
    public:
        
        FQueue(FVulkanRenderContext* InRenderContext, VkQueue InQueue, uint32 InQueueFamilyIndex, ECommandQueue InType);
        ~FQueue() override;
        
        TRefCountPtr<FTrackedCommandBuffer> GetOrCreateCommandBuffer();

        void RetireCommandBuffers();
        
        uint64 Submit(ICommandList* const* CommandLists, uint32 NumCommandLists);

        void WaitIdle();
        uint64 UpdateLastFinishID();
        bool PollCommandList(uint64 CommandListID);
        bool WaitCommandList(uint64 CommandListID, uint64 Timeout);
        
        void AddSignalSemaphore(VkSemaphore Semaphore, uint64 Value);
        void AddWaitSemaphore(VkSemaphore Semaphore, uint64 Value);

        uint64                      LastRecordingID = 0;
        uint64                      LastSubmittedID = 0;
        uint64                      LastFinishedID = 0;
        
        TVector<VkSemaphore>        WaitSemaphores;
        TVector<uint64>             WaitSemaphoreValues;
        TVector<VkSemaphore>        SignalSemaphores;
        TVector<uint64>             SignalSemaphoreValues;
        
        ECommandQueue               Type;
        FMutex                      GetMutex;
        FMutex                      SubmitMutex;
        VkCommandPool               CommandPool;
        VkQueue                     Queue;
        uint32                      QueueFamilyIndex;
        VkSemaphore                 TimelineSemaphore;

        TFixedVector<TRefCountPtr<FTrackedCommandBuffer>, 4> CommandBuffersInFlight;
        TQueue<TRefCountPtr<FTrackedCommandBuffer>> CommandBufferPool;
    };
    
    class FVulkanRenderContext : public IRenderContext
    {
    public:
        
        FVulkanRenderContext();
        
        bool Initialize() override;
        void Deinitialize() override;

        void SetVSyncEnabled(bool bEnable) override;
        bool IsVSyncEnabled() const override;

        void WaitIdle() override;
        
        bool FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) override;
        bool FrameEnd(const FUpdateContext& UpdateContext) override;

        NODISCARD FQueue* GetQueue(ECommandQueue Type) const { return Queues[(uint32)Type]; }

        NODISCARD FRHICommandListRef CreateCommandList(const FCommandListInfo& Info) override;
        uint64 ExecuteCommandLists(ICommandList* const* CommandLists, uint32 NumCommandLists, ECommandQueue QueueType) override;
        NODISCARD FRHICommandListRef GetCommandList(ECommandQueue Queue) override;
        
        void CreateDevice(vkb::Instance Instance);

        NODISCARD VkInstance GetVulkanInstance() const { return VulkanInstance; }
        NODISCARD FVulkanDevice* GetDevice() const { return VulkanDevice; }
        NODISCARD FVulkanSwapchain* GetSwapchain() const { return Swapchain; }
        
        //-------------------------------------------------------------------------------------

        NODISCARD FRHIEventQueryRef CreateEventQuery() override;
        void SetEventQuery(IEventQuery* Query, ECommandQueue Queue) override;
        void ResetEventQuery(IEventQuery* Query) override;
        void PollEventQuery(IEventQuery* Query) override;
        void WaitEventQuery(IEventQuery* Query) override;

        //-------------------------------------------------------------------------------------


        NODISCARD FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Description) override;
        NODISCARD uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage) override;

        
        //-------------------------------------------------------------------------------------

        NODISCARD FRHIViewportRef CreateViewport(const FIntVector2D& Size) override;
        

        NODISCARD FRHIImageRef CreateImage(const FRHIImageDesc& ImageSpec) override;
        NODISCARD FRHISamplerRef CreateSampler(const FSamplerDesc& SamplerDesc) override;
        
        
        //-------------------------------------------------------------------------------------

        NODISCARD FRHIVertexShaderRef CreateVertexShader(const TVector<uint32>& ByteCode) override;
        NODISCARD FRHIPixelShaderRef CreatePixelShader(const TVector<uint32>& ByteCode) override;
        NODISCARD FRHIComputeShaderRef CreateComputeShader(const TVector<uint32>& ByteCode) override;

        NODISCARD IShaderCompiler* GetShaderCompiler() const override;
        NODISCARD FRHIShaderLibraryRef GetShaderLibrary() const override;
        void CompileEngineShaders() override;
        void OnShaderCompiled(FRHIShader* Shader) override;

        
        //-------------------------------------------------------------------------------------

        NODISCARD FRHIDescriptorTableRef CreateDescriptorTable(FRHIBindingLayout* InLayout) override;
        void ResizeDescriptorTable(FRHIDescriptorTable* Table, uint32 NewSize, bool bKeepContents) override;
        bool WriteDescriptorTable(FRHIDescriptorTable* Table, const FBindingSetItem& Binding) override;
        NODISCARD FRHIInputLayoutRef CreateInputLayout(const FVertexAttributeDesc* AttributeDesc, uint32 Count) override;
        NODISCARD FRHIBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc& Desc) override;
        NODISCARD FRHIBindingLayoutRef CreateBindlessLayout(const FBindlessLayoutDesc& Desc) override;
        NODISCARD FRHIBindingSetRef CreateBindingSet(const FBindingSetDesc& Desc, FRHIBindingLayout* InLayout) override;
        NODISCARD FRHIComputePipelineRef CreateComputePipeline(const FComputePipelineDesc& Desc) override;
        NODISCARD FRHIGraphicsPipelineRef CreateGraphicsPipeline(const FGraphicsPipelineDesc& Desc) override;
        

        //-------------------------------------------------------------------------------------

        void SetObjectName(IRHIResource* Resource, const char* Name, EAPIResourceType Type) override;
        
        void FlushPendingDeletes() override;
        
        void SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle);
        FVulkanRenderContextFunctions& GetDebugUtils();

    
    private:

        THashMap<uint64, FRHIInputLayoutRef>            InputLayoutMap;
        THashMap<uint64, FRHISamplerRef>                SamplerMap;
        
        FVulkanDescriptorCache                          DescriptorCache;
        FVulkanPipelineCache                            PipelineCache;
        uint8                                           CurrentFrameIndex;
        TArray<FQueue*, (uint32)ECommandQueue::Num>     Queues;
        
        FRHICommandListRef                              GraphicsCommandList;
        FRHICommandListRef                              ComputeCommandList;
        FRHICommandListRef                              TransferCommandList;

        VkInstance                                      VulkanInstance;
        
        FVulkanSwapchain*                               Swapchain = nullptr;
        FVulkanDevice*                                  VulkanDevice = nullptr;
        FVulkanRenderContextFunctions                   DebugUtils;

        FSpirVShaderCompiler*                           ShaderCompiler;
        FRHIShaderLibraryRef                            ShaderLibrary;
    };
    
}
