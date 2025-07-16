

#include <random>
#include "VulkanCommandList.h"
#include "VulkanDevice.h"
#include "Core/Profiler/Profile.h"
#include "Paths/Paths.h"
#include "Renderer/CommandList.h"
#include "Renderer/ShaderCompiler.h"
#include "TaskSystem/TaskSystem.h"
#ifdef LUMINA_RENDERER_VULKAN

#include "VulkanResources.h"
#include "VulkanSwapchain.h"
#include "Core/Windows/Window.h"
#include "..\..\StateTracking.h"


#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "VulkanMacros.h"
#include "Core/Engine/Engine.h"
#include "Core/Math/Alignment.h"
#include "VulkanRenderContext.h"

#include "Renderer/RenderManager.h"
#include "src/VkBootstrap.h"

namespace Lumina
{
    extern TStack<IRHIResource*, TFixedVector<IRHIResource*, 100>> PendingDeletes;

    VkBool32 VKAPI_PTR VkDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        // Helper to decode messageTypes
        auto GetMessageTypeString = [](VkDebugUtilsMessageTypeFlagsEXT types) -> std::string {
            std::string result;
            if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
                result += "[General] ";
            }
            if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
                result += "[Validation] ";
            }
            if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
                result += "[Performance] ";
            }
            return result.empty() ? "[Unknown] " : result;
        };

        std::string typeStr = GetMessageTypeString(messageTypes);

        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE("Vulkan {}{}", typeStr, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_DEBUG("Vulkan {}{}", typeStr, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN("Vulkan {}{}", typeStr, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR("Vulkan {}{}", typeStr, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            std::unreachable();
            break;
        }

        return VK_FALSE;
    }

        constexpr VkObjectType ToVkObjectType(EAPIResourceType type)
    {
        switch (type)
        {
        case EAPIResourceType::Buffer: return VK_OBJECT_TYPE_BUFFER;
        case EAPIResourceType::Image: return VK_OBJECT_TYPE_IMAGE;
        case EAPIResourceType::ImageView: return VK_OBJECT_TYPE_IMAGE_VIEW;
        case EAPIResourceType::Sampler: return VK_OBJECT_TYPE_SAMPLER;
        case EAPIResourceType::ShaderModule: return VK_OBJECT_TYPE_SHADER_MODULE;
        case EAPIResourceType::Pipeline: return VK_OBJECT_TYPE_PIPELINE;
        case EAPIResourceType::PipelineLayout: return VK_OBJECT_TYPE_PIPELINE_LAYOUT;
        case EAPIResourceType::RenderPass: return VK_OBJECT_TYPE_RENDER_PASS;
        case EAPIResourceType::Framebuffer: return VK_OBJECT_TYPE_FRAMEBUFFER;
        case EAPIResourceType::DescriptorSet: return VK_OBJECT_TYPE_DESCRIPTOR_SET;
        case EAPIResourceType::DescriptorSetLayout: return VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
        case EAPIResourceType::DescriptorPool: return VK_OBJECT_TYPE_DESCRIPTOR_POOL;
        case EAPIResourceType::CommandPool: return VK_OBJECT_TYPE_COMMAND_POOL;
        case EAPIResourceType::CommandBuffer: return VK_OBJECT_TYPE_COMMAND_BUFFER;
        case EAPIResourceType::Semaphore: return VK_OBJECT_TYPE_SEMAPHORE;
        case EAPIResourceType::Fence: return VK_OBJECT_TYPE_FENCE;
        case EAPIResourceType::Event: return VK_OBJECT_TYPE_EVENT;
        case EAPIResourceType::QueryPool: return VK_OBJECT_TYPE_QUERY_POOL;
        case EAPIResourceType::DeviceMemory: return VK_OBJECT_TYPE_DEVICE_MEMORY;
        case EAPIResourceType::Swapchain: return VK_OBJECT_TYPE_SWAPCHAIN_KHR;
        case EAPIResourceType::Surface: return VK_OBJECT_TYPE_SURFACE_KHR;
        case EAPIResourceType::Device: return VK_OBJECT_TYPE_DEVICE;
        case EAPIResourceType::Instance: return VK_OBJECT_TYPE_INSTANCE;
        case EAPIResourceType::Queue: return VK_OBJECT_TYPE_QUEUE;
        default: return VK_OBJECT_TYPE_UNKNOWN;
        }
    }
    
    //------------------------------------------------------------------------------------


    FQueue::FQueue(FVulkanRenderContext* InRenderContext, VkQueue InQueue, uint32 InQueueFamilyIndex, ECommandQueue InType)
        : IDeviceChild(InRenderContext->GetDevice())
        , Type(InType)
        , CommandPool(nullptr)
        , Queue(InQueue)
        , QueueFamilyIndex(InQueueFamilyIndex)
    {
        VkSemaphoreTypeCreateInfo TimelineInfo = {};
        TimelineInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        TimelineInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        TimelineInfo.initialValue = 0;

        VkSemaphoreCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        CreateInfo.pNext = &TimelineInfo;

        VK_CHECK(vkCreateSemaphore(Device->GetDevice(), &CreateInfo, nullptr, &TimelineSemaphore));
        InRenderContext->SetVulkanObjectName("Timeline Semaphore", VK_OBJECT_TYPE_SEMAPHORE, (uintptr_t)TimelineSemaphore);
    }
    
    FQueue::~FQueue()
    {
        vkDestroySemaphore(Device->GetDevice(), TimelineSemaphore, nullptr);
        TimelineSemaphore = nullptr;
    }

    TRefCountPtr<FTrackedCommandBuffer> FQueue::GetOrCreateCommandBuffer()
    {
        LUMINA_PROFILE_SCOPE();
        
        FScopeLock Lock(Mutex);

        uint64 RecodingID = ++LastRecordingID;
        
        TRefCountPtr<FTrackedCommandBuffer> Buf;
        if (CommandBufferPool.empty())
        {
            VkCommandPoolCreateFlags Flags = 0;
            Flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        
            VkCommandPoolCreateInfo PoolInfo = {};
            PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            PoolInfo.queueFamilyIndex = QueueFamilyIndex;
            PoolInfo.flags = Flags;
        
            VK_CHECK(vkCreateCommandPool(Device->GetDevice(), &PoolInfo, nullptr, &CommandPool));

            VkCommandBufferAllocateInfo BufferInfo = {};
            BufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            BufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            BufferInfo.commandBufferCount = 1;
            BufferInfo.commandPool = CommandPool;

            VkCommandBuffer Buffer;
            VK_CHECK(vkAllocateCommandBuffers(Device->GetDevice(), &BufferInfo, &Buffer));

            bool bCreateTracy = (Type == ECommandQueue::Graphics || Type == ECommandQueue::Compute);
            Buf = MakeRefCount<FTrackedCommandBuffer>(Device, Buffer, CommandPool, bCreateTracy, Queue);
        }
        else
        {
            Buf = CommandBufferPool.front();
            CommandBufferPool.pop();
        }

        Buf->RecordingID = RecodingID;
        return Buf;
    }

    void FQueue::RetireCommandBuffers()
    {
        LUMINA_PROFILE_SCOPE();
        
        auto Submissions = Memory::Move(CommandBuffersInFlight);
        CommandBuffersInFlight.clear();

        uint64 LastFinish = UpdateLastFinishID();
        
        for (auto& Submission : Submissions)
        {
            if (Submission->SubmissionID <= LastFinish)
            {
                Submission->ClearReferencedResources();
                Submission->SubmissionID = 0;
                CommandBufferPool.push(Submission);
            }
            else
            {
                CommandBuffersInFlight.push_back(Submission);
            }
        }
    }

    uint64 FQueue::Submit(ICommandList* CommandLists, uint32 NumCommandLists)
    {
        LUMINA_PROFILE_SCOPE();

        FScopeLock Lock(Mutex);
        
        TFixedVector<VkCommandBuffer, 4> CommandBuffers(NumCommandLists);
        TFixedVector<VkPipelineStageFlags, 4> StageFlags(WaitSemaphores.size());

        for (SIZE_T i = 0; i < WaitSemaphores.size(); ++i)
        {
            StageFlags[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
        
        LastSubmittedID++;

        for (uint32 i = 0; i < NumCommandLists; ++i)
        {
            auto* VulkanCommandList = static_cast<FVulkanCommandList*>(&CommandLists[i]);
            auto& TrackedBuffer = VulkanCommandList->CurrentCommandBuffer;

            Assert(TrackedBuffer->Queue == Queue)

            CommandBuffers[i] = TrackedBuffer->CommandBuffer;
            TrackedBuffer->LastCommandListID = LastSubmittedID;
            CommandBuffersInFlight.push_back(TrackedBuffer);

            for (const auto& Buffer : TrackedBuffer->ReferencedStagingResources)
            {
                FVulkanBuffer* VkBuf = Buffer.As<FVulkanBuffer>();
                VkBuf->LastUseQueue = Type;
                VkBuf->LastUseCommandListID = LastSubmittedID;
            }
            
        }

        AddSignalSemaphore(TimelineSemaphore, LastSubmittedID);

        VkTimelineSemaphoreSubmitInfo TimelineSubmitInfo = {};
        TimelineSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        TimelineSubmitInfo.signalSemaphoreValueCount = (uint32)SignalSemaphoreValues.size();
        TimelineSubmitInfo.pSignalSemaphoreValues = SignalSemaphoreValues.data();
        if (!WaitSemaphoreValues.empty())
        {
            TimelineSubmitInfo.waitSemaphoreValueCount = (uint32)WaitSemaphoreValues.size();
            TimelineSubmitInfo.pWaitSemaphoreValues = WaitSemaphoreValues.data();
        }


        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.pNext = &TimelineSubmitInfo;
        SubmitInfo.pCommandBuffers = CommandBuffers.data();
        SubmitInfo.commandBufferCount = NumCommandLists;
        SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
        SubmitInfo.waitSemaphoreCount = (uint32)WaitSemaphores.size();
        SubmitInfo.pWaitDstStageMask = StageFlags.data();
        SubmitInfo.pSignalSemaphores = SignalSemaphores.data();
        SubmitInfo.signalSemaphoreCount = (uint32)SignalSemaphores.size();

        VK_CHECK(vkQueueSubmit(Queue, 1, &SubmitInfo, nullptr));

        WaitSemaphores.clear();
        WaitSemaphoreValues.clear();
        SignalSemaphores.clear();
        SignalSemaphoreValues.clear();

        return LastSubmittedID;
    }

    void FQueue::WaitIdle()
    {
        LUMINA_PROFILE_SCOPE();
        VK_CHECK(vkQueueWaitIdle(Queue));
    }

    uint64 FQueue::UpdateLastFinishID()
    {
        VK_CHECK(vkGetSemaphoreCounterValue(Device->GetDevice(), TimelineSemaphore, &LastFinishedID));
        return LastFinishedID;
    }

    bool FQueue::PollCommandList(uint64 CommandListID)
    {
        if (CommandListID > LastSubmittedID || CommandListID == 0)
        {
            return false;
        }

        bool bCompleted = LastFinishedID >= CommandListID;
        if (bCompleted)
        {
            return true;
        }
        
        bCompleted = UpdateLastFinishID() >= CommandListID;
        return bCompleted;
    }

    bool FQueue::WaitCommandList(uint64 CommandListID, uint64 Timeout)
    {
        if (CommandListID > LastSubmittedID || CommandListID == 0)
        {
            return false;
        }

        if (PollCommandList(CommandListID))
        {
            return true;
        }

        VkSemaphoreWaitInfo WaitInfo = {};
        WaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
        WaitInfo.semaphoreCount = 1;
        WaitInfo.pSemaphores = &TimelineSemaphore;
        WaitInfo.pValues = &CommandListID;

        VkResult Result = vkWaitSemaphores(Device->GetDevice(), &WaitInfo, Timeout);
        VK_CHECK(Result);
        
        return (Result == VK_SUCCESS);
    }

    void FQueue::AddSignalSemaphore(VkSemaphore Semaphore, uint64 Value)
    {
        Assert(Semaphore)
        SignalSemaphores.push_back(Semaphore);
        SignalSemaphoreValues.push_back(Value);
    }

    void FQueue::AddWaitSemaphore(VkSemaphore Semaphore, uint64 Value)
    {
        Assert(Semaphore)
        WaitSemaphores.push_back(Semaphore);
        WaitSemaphoreValues.push_back(Value);
    }
    
    FVulkanRenderContext::FVulkanRenderContext()
        : CurrentFrameIndex(0)
        , Queues{}
        , VulkanInstance(nullptr)
        , ShaderCompiler(nullptr)
    {
    }

    void FVulkanRenderContext::Initialize()
    {
        LUMINA_PROFILE_SCOPE();
        
        AssertMsg(glfwVulkanSupported(), "Vulkan Is Not Supported!");
        
        vkb::InstanceBuilder Builder;
        auto InstBuilder = Builder
        .set_app_name("Lumina Engine")
        //.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT)
        .add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT)
        //.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT)
        //.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT)
        //.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT)
        .request_validation_layers()
        .use_default_debug_messenger()
        .set_debug_callback(VkDebugCallback)
        .enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
        .require_api_version(1, 3, 0)
        .build();

        VulkanInstance = InstBuilder.value();
        
        DebugUtils.DebugMessenger = InstBuilder->debug_messenger;
        DebugUtils.DebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(vkGetInstanceProcAddr(
            VulkanInstance, "vkSetDebugUtilsObjectNameEXT"));

        
        CreateDevice(InstBuilder.value());
        
        DebugUtils.vkCmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(GetDevice()->GetDevice(), "vkCmdDebugMarkerBeginEXT");
        DebugUtils.vkCmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(GetDevice()->GetDevice(), "vkCmdDebugMarkerEndEXT");

        CommandList = CreateCommandList(FCommandListInfo());
        
        Swapchain = Memory::New<FVulkanSwapchain>();
        Swapchain->CreateSwapchain(VulkanInstance, this, Windowing::GetPrimaryWindowHandle(), Windowing::GetPrimaryWindowHandle()->GetExtent());

        ShaderLibrary = MakeRefCount<FShaderLibrary>();
        ShaderCompiler = Memory::New<FSpirVShaderCompiler>();
        ShaderCompiler->Initialize();
            
        CompileEngineShaders();
        
        WaitIdle();
        FlushPendingDeletes();

    }

    void FVulkanRenderContext::Deinitialize()
    {
        WaitIdle();
        
        CommandList.SafeRelease();
        ShaderLibrary.SafeRelease();
        PipelineCache.ReleasePipelines();
        DescriptorCache.ReleaseResources();

        ShaderCompiler->Shutdown();
        Memory::Delete(ShaderCompiler);
        
        Memory::Delete(Swapchain);
        
        for (size_t i = 0; i < Queues.size(); ++i)
        {
            Memory::Delete(Queues[i]);
            Queues[i] = nullptr;
        }
        
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        func(VulkanInstance, DebugUtils.DebugMessenger, nullptr);
        
        FlushPendingDeletes();
        
        Memory::Delete(VulkanDevice);
        vkDestroyInstance(VulkanInstance, nullptr);
    }

    void FVulkanRenderContext::SetVSyncEnabled(bool bEnable)
    {
        Swapchain->SetPresentMode(bEnable ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
    }

    bool FVulkanRenderContext::IsVSyncEnabled() const
    {
        return Swapchain->GetPresentMode() == VK_PRESENT_MODE_FIFO_KHR;
    }

    void FVulkanRenderContext::WaitIdle()
    {
        VK_CHECK(vkDeviceWaitIdle(VulkanDevice->GetDevice()));
    }

    bool FVulkanRenderContext::FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex)
    {
        CurrentFrameIndex = InCurrentFrameIndex;

        bool bSuccess = Swapchain->AcquireNextImage();
        
        CommandList->Open();
        
        return bSuccess;
    }

    bool FVulkanRenderContext::FrameEnd(const FUpdateContext& UpdateContext)
    {
        CommandList->CopyImage(GEngine->GetEngineViewport()->GetRenderTarget(), {}, Swapchain->GetCurrentImage(), {});
        
        CommandList->Close();
        
        ExecuteCommandList(CommandList, 1, Q_Graphics);
        
        bool bSuccess = Swapchain->Present();
        
        FlushPendingDeletes();

        return bSuccess;
    }

    FRHICommandListRef FVulkanRenderContext::CreateCommandList(const FCommandListInfo& Info)
    {

        if (Queues[uint32(Info.CommandQueue)] == nullptr)
        {
            return nullptr;
        }

        return MakeRefCount<FVulkanCommandList>(this, Info);
    }
    
    void FVulkanRenderContext::ExecuteCommandList(ICommandList* CommandLists, uint32 NumCommandLists, ECommandQueue QueueType)
    {
        FQueue* Queue = Queues[uint32(QueueType)];

        uint64 SubmissionID = Queue->Submit(CommandLists, NumCommandLists);

        for (uint32 i = 0; i < NumCommandLists; ++i)
        {
            /** Static cast in this case if fine because CommandLists will only ever contain one type. */
            static_cast<FVulkanCommandList*>(&CommandLists[i])->Executed(Queue, SubmissionID);
        }
    }

    FRHICommandListRef FVulkanRenderContext::GetCommandList(ECommandQueue Queue)
    {
        return CommandList;
    }

    void FVulkanRenderContext::CreateDevice(vkb::Instance Instance)
    {
        VkPhysicalDeviceTimelineSemaphoreFeatures TimelineFeatures = {};
        TimelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
        TimelineFeatures.timelineSemaphore = VK_TRUE;
        
        VkPhysicalDeviceFeatures DeviceFeatures = {};
        DeviceFeatures.samplerAnisotropy = VK_TRUE;
        DeviceFeatures.sampleRateShading = VK_TRUE;
        DeviceFeatures.fillModeNonSolid = VK_TRUE;
        DeviceFeatures.wideLines = VK_TRUE;
        
        
        VkPhysicalDeviceVulkan12Features Features12 = {};
        Features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        Features12.timelineSemaphore = VK_TRUE;
        Features12.bufferDeviceAddress = VK_TRUE;
        Features12.descriptorIndexing =  VK_TRUE;
        Features12.descriptorBindingPartiallyBound = VK_TRUE;

        VkPhysicalDeviceVulkan13Features Features13 = {};
        Features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        Features13.dynamicRendering = VK_TRUE;
        Features13.synchronization2 = VK_TRUE;
        
        vkb::PhysicalDeviceSelector selector(Instance);
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features(DeviceFeatures)
            .set_required_features_12(Features12)
            .set_required_features_13(Features13)
            .require_separate_transfer_queue()
            .require_separate_compute_queue()
            .defer_surface_initialization()
            .select()
            .value();
        

        physicalDevice.enable_extension_if_present(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        physicalDevice.enable_extension_if_present(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME);

        vkb::DeviceBuilder deviceBuilder(physicalDevice);
        vkb::Device vkbDevice = deviceBuilder.build().value();

        
        VkDevice Device = vkbDevice.device;
        VkPhysicalDevice PhysicalDevice = physicalDevice.physical_device;
        VulkanDevice = Memory::New<FVulkanDevice>(this, VulkanInstance, PhysicalDevice, Device);

        if (vkbDevice.get_queue(vkb::QueueType::graphics).has_value())
        {
            VkQueue Queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
            uint32 Index = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
            Queues[uint32(ECommandQueue::Graphics)] = Memory::New<FQueue>(this, Queue, Index, ECommandQueue::Graphics);
            SetVulkanObjectName("Graphics Queue", VK_OBJECT_TYPE_QUEUE, (uintptr_t)Queue);
        }

        if (vkbDevice.get_queue(vkb::QueueType::compute).has_value())
        {
            VkQueue Queue = vkbDevice.get_queue(vkb::QueueType::compute).value();
            uint32 Index = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
            Queues[uint32(ECommandQueue::Compute)] = Memory::New<FQueue>(this, Queue, Index, ECommandQueue::Compute);
            SetVulkanObjectName("Compute Queue", VK_OBJECT_TYPE_QUEUE, (uintptr_t)Queue);
        }

        if (vkbDevice.get_queue(vkb::QueueType::transfer).has_value())
        {
            VkQueue Queue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
            uint32 Index = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
            Queues[uint32(ECommandQueue::Transfer)] = Memory::New<FQueue>(this, Queue, Index, ECommandQueue::Transfer);
            SetVulkanObjectName("Transfer Queue", VK_OBJECT_TYPE_QUEUE, (uintptr_t)Queue);
        }
    }

    uint64 FVulkanRenderContext::GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage)
    {
        uint64 MinAlignment = 1;

        if(Usage.AreAnyFlagsSet(EBufferUsageFlags::Dynamic))
        {
            MinAlignment = VulkanDevice->GetPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
        }

        return Math::GetAligned(Size, MinAlignment);
    }
    
    FRHIViewportRef FVulkanRenderContext::CreateViewport(const FIntVector2D& Size)
    {
        return MakeRefCount<FVulkanViewport>(Size, this);
    }

    FRHIImageRef FVulkanRenderContext::CreateImage(const FRHIImageDesc& ImageSpec)
    {
        return MakeRefCount<FVulkanImage>(VulkanDevice, ImageSpec);
    }

    FRHISamplerRef FVulkanRenderContext::CreateSampler(const FSamplerDesc& SamplerDesc)
    {
        return MakeRefCount<FVulkanSampler>(VulkanDevice, SamplerDesc);
    }

    FRHIVertexShaderRef FVulkanRenderContext::CreateVertexShader(const TVector<uint32>& ByteCode)
    {
        return MakeRefCount<FVulkanVertexShader>(VulkanDevice, ByteCode);
    }

    FRHIPixelShaderRef FVulkanRenderContext::CreatePixelShader(const TVector<uint32>& ByteCode)
    {
        return MakeRefCount<FVulkanPixelShader>(VulkanDevice, ByteCode);
    }

    FRHIComputeShaderRef FVulkanRenderContext::CreateComputeShader(const TVector<uint32>& ByteCode)
    {
        return MakeRefCount<FVulkanComputeShader>(VulkanDevice, ByteCode);
    }

    IShaderCompiler* FVulkanRenderContext::GetShaderCompiler() const
    {
        return ShaderCompiler;
    }

    FRHIShaderLibraryRef FVulkanRenderContext::GetShaderLibrary() const
    {
        return ShaderLibrary;
    }

    FRHIDescriptorTableRef FVulkanRenderContext::CreateDescriptorTable(FRHIBindingLayout* InLayout)
    {
        return MakeRefCount<FVulkanDescriptorTable>(this, (FVulkanBindingLayout*)InLayout);
    }

    void FVulkanRenderContext::ResizeDescriptorTable(FRHIDescriptorTable* Table, uint32 NewSize, bool bKeepContents)
    {
        (void)Table;
        (void)NewSize;
        (void)bKeepContents;
    }

    bool FVulkanRenderContext::WriteDescriptorTable(FRHIDescriptorTable* Table, const FBindingSetItem& Binding)
    {
        FVulkanDescriptorTable* DescriptorTable = static_cast<FVulkanDescriptorTable*>(Table);
        FVulkanBindingLayout* BindingLayout = static_cast<FVulkanBindingLayout*>(DescriptorTable->GetLayout());

        TVector<VkWriteDescriptorSet> Writes;
        TVector<VkDescriptorImageInfo> ImageWriteInfos;
        TVector<VkDescriptorBufferInfo> BufferWriteInfos;
        
        if (Binding.Slot >= DescriptorTable->GetCapacity())
        {
            return false;
        }


        auto WriteDescriptorForBinding = [&] (const VkDescriptorSetLayoutBinding& LayoutBinding)
        {
            VkWriteDescriptorSet Write = {};
            Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            Write.descriptorCount = LayoutBinding.descriptorCount;
            Write.dstArrayElement = 0;
            Write.dstBinding = LayoutBinding.binding;
            Write.dstSet = DescriptorTable->DescriptorSet;
            
            switch (Binding.Type)
            {
            case ERHIBindingResourceType::Texture_SRV:
                {
                    FVulkanImage* Image = static_cast<FVulkanImage*>(Binding.ResourceHandle);
                    VkDescriptorImageInfo& ImageInfo = ImageWriteInfos.emplace_back();
                    ImageInfo.imageView = Image->GetImageView();
                    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    ImageInfo.sampler = GEngine->GetEngineSubsystem<FRenderManager>()->GetLinearSampler()->GetAPIResource<VkSampler>();

                    
                    Write.pImageInfo = &ImageInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                }
                break;
            case ERHIBindingResourceType::Texture_UAV:
                {
                    FVulkanImage* Image = static_cast<FVulkanImage*>(Binding.ResourceHandle);
                    VkDescriptorImageInfo& ImageInfo = ImageWriteInfos.emplace_back();
                    ImageInfo.imageView = Image->GetImageView();
                    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    Write.pImageInfo = &ImageInfo;
                }
                break;
            case ERHIBindingResourceType::Buffer_CBV:
                {
                    FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Binding.ResourceHandle);
                    VkDescriptorBufferInfo& BufferInfo = BufferWriteInfos.emplace_back();
                    BufferInfo.buffer = Buffer->GetBuffer();
                    BufferInfo.offset = 0;
                    BufferInfo.range = Buffer->GetSize();
                        
                    Write.pBufferInfo = &BufferInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                }
                break;
            case ERHIBindingResourceType::Buffer_SRV:
                {
                    FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Binding.ResourceHandle);
                    VkDescriptorBufferInfo& BufferInfo = BufferWriteInfos.emplace_back();
                    BufferInfo.buffer = Buffer->GetBuffer();
                    BufferInfo.offset = 0;
                    BufferInfo.range = Buffer->GetSize();
                        
                    Write.pBufferInfo = &BufferInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                }
                break;
            case ERHIBindingResourceType::Buffer_UAV:
                {
                    FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Binding.ResourceHandle);
                    VkDescriptorBufferInfo& BufferInfo = BufferWriteInfos.emplace_back();
                    BufferInfo.buffer = Buffer->GetBuffer();
                    BufferInfo.offset = 0;
                    BufferInfo.range = Buffer->GetSize();
                        
                    Write.pBufferInfo = &BufferInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                }
                break;
            }

            Writes.push_back(Write);
        };

        for (uint32 BindingLocation = 0; BindingLocation < uint32(BindingLayout->Bindings.size()); BindingLocation++)
        {
            if (BindingLayout->BindlessDesc.Bindings[BindingLocation].Type == Binding.Type)
            {
                const VkDescriptorSetLayoutBinding& LayoutBinding = BindingLayout->Bindings[BindingLocation];
                WriteDescriptorForBinding(LayoutBinding);
            }
        }
        
        vkUpdateDescriptorSets(VulkanDevice->GetDevice(), (uint32)Writes.size(), Writes.data(), 0, nullptr);

        return true;
    }
    
    FRHIBindingLayoutRef FVulkanRenderContext::CreateBindingLayout(const FBindingLayoutDesc& Desc)
    {
        auto Layout = MakeRefCount<FVulkanBindingLayout>(VulkanDevice, Desc);
        
        Layout->Bake();

        return Layout;
    }

    FRHIBindingLayoutRef FVulkanRenderContext::CreateBindlessLayout(const FBindlessLayoutDesc& Desc)
    {
        auto Layout = MakeRefCount<FVulkanBindingLayout>(VulkanDevice, Desc);
        
        Layout->Bake();

        return Layout;
    }

    FRHIBindingSetRef FVulkanRenderContext::CreateBindingSet(const FBindingSetDesc& Desc, FRHIBindingLayout* InLayout)
    {
        return MakeRefCount<FVulkanBindingSet>(this, Desc, (FVulkanBindingLayout*)InLayout);
    }

    FRHIComputePipelineRef FVulkanRenderContext::CreateComputePipeline(const FComputePipelineDesc& Desc)
    {
        return PipelineCache.GetOrCreateComputePipeline(VulkanDevice, Desc);
    }

    FRHIGraphicsPipelineRef FVulkanRenderContext::CreateGraphicsPipeline(const FGraphicsPipelineDesc& Desc)
    {
        return PipelineCache.GetOrCreateGraphicsPipeline(VulkanDevice, Desc);
    }

    void FVulkanRenderContext::SetObjectName(IRHIResource* Resource, const char* Name, EAPIResourceType Type)
    {
        if (GetDebugUtils().DebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT NameInfo = {};
            NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            NameInfo.pObjectName = Name;
            NameInfo.objectType = ToVkObjectType(Type);
            NameInfo.objectHandle = reinterpret_cast<uint64>(Resource->GetAPIResource(Type));

            GetDebugUtils().DebugUtilsObjectNameEXT(GetDevice()->GetDevice(), &NameInfo);
        }
    }

    FRHIEventQueryRef FVulkanRenderContext::CreateEventQuery()
    {
        return MakeRefCount<FVulkanEventQuery>();
    }

    void FVulkanRenderContext::SetEventQuery(IEventQuery* Query, ECommandQueue Queue)
    {
        FVulkanEventQuery* VkQuery = static_cast<FVulkanEventQuery*>(Query);
        Assert(VkQuery->CommandListID == 0)

        VkQuery->Queue = Queue;
        VkQuery->CommandListID = GetQueue(Queue)->LastSubmittedID;
    }

    void FVulkanRenderContext::ResetEventQuery(IEventQuery* Query)
    {
        FVulkanEventQuery* VkQuery = static_cast<FVulkanEventQuery*>(Query);
        VkQuery->CommandListID = 0;
    }

    void FVulkanRenderContext::PollEventQuery(IEventQuery* Query)
    {
        FVulkanEventQuery* VkQuery = static_cast<FVulkanEventQuery*>(Query);
        FQueue* Queue = GetQueue(VkQuery->Queue);
        Queue->PollCommandList(VkQuery->CommandListID);
    }

    void FVulkanRenderContext::WaitEventQuery(IEventQuery* Query)
    {
        FVulkanEventQuery* VkQuery = static_cast<FVulkanEventQuery*>(Query);
        if (VkQuery->CommandListID == 0)
        {
            return;
        }

        FQueue* Queue = GetQueue(VkQuery->Queue);
        bool bSuccess = Queue->WaitCommandList(VkQuery->CommandListID, UINT64_MAX);
        Assert(bSuccess);
        
        (void)bSuccess;
    }

    FRHIBufferRef FVulkanRenderContext::CreateBuffer(const FRHIBufferDesc& Description)
    {
        return MakeRefCount<FVulkanBuffer>(VulkanDevice, Description);
    }
    
    void FVulkanRenderContext::FlushPendingDeletes()
    {
        LUMINA_PROFILE_SCOPE();
        
        for (FQueue* Queue : Queues)
        {
            if (Queue != nullptr)
            {
                Queue->RetireCommandBuffers();
            }
        }

        while (!PendingDeletes.empty())
        {
            IRHIResource* Resource = PendingDeletes.top();
            PendingDeletes.pop();
            if (Resource->Deleting())
            {
                Memory::Delete(Resource);
            }
        }
    }


    void FVulkanRenderContext::CompileEngineShaders()
    {
        for (auto& Dir : std::filesystem::directory_iterator(Paths::GetEngineResourceDirectory() / "Shaders"))
        {
            if (!Dir.is_directory())
            {
                if (Dir.path().extension() == ".frag" || Dir.path().extension() == ".vert" || Dir.path().extension() == ".comp")
                {
                    FString StringPath = Dir.path().string().c_str();

                    bool bSuccess = ShaderCompiler->CompileShader(StringPath, {}, [&, Dir = Memory::Move(Dir)] (const TVector<uint32>& Binaries)
                    {
                        FString FileNameString = Dir.path().filename().string().c_str();
                        
                        if (Dir.path().extension() == ".vert")
                        {
                            FRHIVertexShaderRef Shader = CreateVertexShader(Binaries);
                            Shader->SetKey(FileNameString.c_str());
                            ShaderLibrary->AddShader(Shader);
                            PipelineCache.PostShaderRecompiled(Shader.As<FVulkanVertexShader>());

                        }
                        else if (Dir.path().extension() == ".frag")
                        {
                            FRHIPixelShaderRef Shader = CreatePixelShader(Binaries);
                            Shader->SetKey(FileNameString.c_str());
                            ShaderLibrary->AddShader(Shader);
                            PipelineCache.PostShaderRecompiled(Shader.As<FVulkanPixelShader>());
                        }
                        else if (Dir.path().extension() == ".comp")
                        {
                            FRHIComputeShaderRef Shader = CreateComputeShader(Binaries);
                            Shader->SetKey(FileNameString.c_str());
                            ShaderLibrary->AddShader(Shader);
                            PipelineCache.PostShaderRecompiled(Shader.As<FVulkanComputeShader>());
                        }
                        
                    });

                    if (!bSuccess)
                    {
                        
                    }
                }
            }
        }
    }

    void FVulkanRenderContext::OnShaderCompiled(FRHIShader* Shader)
    {
        PipelineCache.PostShaderRecompiled((IVulkanShader*)Shader);
    }
    

    FRHIInputLayoutRef FVulkanRenderContext::CreateInputLayout(const FVertexAttributeDesc* AttributeDesc, uint32 Count)
    {
        return MakeRefCount<FVulkanInputLayout>(AttributeDesc, Count);
    }


    void FVulkanRenderContext::SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle)
    {
        if (DebugUtils.DebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT NameInfo = {};
            NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            NameInfo.objectType = ObjectType;
            NameInfo.objectHandle = Handle;
            NameInfo.pObjectName = Name.c_str();

            DebugUtils.DebugUtilsObjectNameEXT(VulkanDevice->GetDevice(), &NameInfo);
        }
    }

    FVulkanRenderContextFunctions& FVulkanRenderContext::GetDebugUtils()
    {
        return DebugUtils;
    }
}


#endif