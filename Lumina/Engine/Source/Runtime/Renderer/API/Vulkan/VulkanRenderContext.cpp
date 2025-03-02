

#include <random>

#include "VulkanCommandList.h"
#include "VulkanDevice.h"
#include "Renderer/CommandList.h"
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
#include "VulkanTypes.h"
#include "Core/Math/Alignment.h"
#include "VulkanRenderContext.h"
#include "src/VkBootstrap.h"

namespace Lumina
{

    extern TStack<IRHIResource*> PendingDeletes;

    VkBool32 VKAPI_PTR VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_INFO("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_DEBUG("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            std::unreachable();
            break;
        }

        return VK_FALSE;
    }
    
    constexpr VkPipelineStageFlagBits ToVkPipelineStage(EPipelineStage stage)
    {
        switch (stage)
        {
            case EPipelineStage::TopOfPipe: return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            case EPipelineStage::BottomOfPipe: return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            
                // Vertex Input stages
            case EPipelineStage::VertexInput: return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            case EPipelineStage::VertexShader: return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            
                // Tessellation stages
            case EPipelineStage::TessellationControlShader: return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
            case EPipelineStage::TessellationEvaluationShader: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
            
                // Geometry stages
            case EPipelineStage::GeometryShader: return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
            
                // Fragment stages
            case EPipelineStage::FragmentShader: return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            case EPipelineStage::ColorAttachmentOutput: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            
                // Compute stages
            case EPipelineStage::ComputeShader: return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            case EPipelineStage::Transfer: return VK_PIPELINE_STAGE_TRANSFER_BIT;
            
                // Post-processing stages
            case EPipelineStage::EarlyFragmentTests: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case EPipelineStage::LateFragmentTests: return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            
                // Special stages
            case EPipelineStage::Host: return VK_PIPELINE_STAGE_HOST_BIT;
            case EPipelineStage::AllGraphics: return VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            case EPipelineStage::AllCommands: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            
            default:
                return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
    }


    VkPipelineStageFlags GetPipelineStageForAccess(VkAccessFlags access)
    {
        VkPipelineStageFlags stageMask = 0;

        if (access & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
        {
            stageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        if (access & VK_ACCESS_TRANSFER_WRITE_BIT)
        {
            stageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        if (access & VK_ACCESS_SHADER_READ_BIT)
        {
            stageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        if (access & VK_ACCESS_SHADER_WRITE_BIT)
        {
            stageMask |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        if (access & VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
        {
            stageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }
        if (access & VK_ACCESS_MEMORY_READ_BIT)
        {
            stageMask |= VK_PIPELINE_STAGE_HOST_BIT;
        }

        return stageMask;
    }
    
    //------------------------------------------------------------------------------------


    TRefCountPtr<FTrackedCommandBufer> FQueue::GetOrCreateCommandBuffer()
    {
        FScopeLock Lock(Mutex);

        TRefCountPtr<FTrackedCommandBufer> Buf;
        
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
            
            Buf = MakeRefCount<FTrackedCommandBufer>(Device, Buffer, CommandPool);    
        }
        else
        {
            Buf = CommandBufferPool.top();
            CommandBufferPool.pop();
        }

        return Buf;
    }

    void FQueue::RetireCommandBuffers()
    {
        TVector<TRefCountPtr<FTrackedCommandBufer>> Submissions = FMemory::Move(CommandBuffersInFlight);

        for (auto& Submission : Submissions)
        {
            Submission->ReferencedResources.clear();
            CommandBufferPool.push(Submission);
        }
    }

    void FQueue::Submit(ICommandList* CommandLists, uint32 NumCommandLists)
    {
        TVector<VkCommandBuffer> CommandBuffers(NumCommandLists);
        
        for (int i = 0; i < NumCommandLists; ++i)
        {
            /** Static cast in this case if fine because CommandLists will only ever contain one type. */
            FVulkanCommandList* VulkanCommandList = static_cast<FVulkanCommandList*>(&CommandLists[i]);

            auto& TrackedBuffer = VulkanCommandList->CurrentCommandBuffer;
            CommandBuffers[i] = TrackedBuffer->CommandBuffer;
            CommandBuffersInFlight.push_back(TrackedBuffer);
        }

        VkPipelineStageFlags WaitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.pWaitDstStageMask = &WaitStages;
        SubmitInfo.pCommandBuffers = CommandBuffers.data();
        SubmitInfo.commandBufferCount = NumCommandLists;
        SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
        SubmitInfo.waitSemaphoreCount = WaitSemaphores.size();
        SubmitInfo.pSignalSemaphores = SignalSemaphores.data();
        SubmitInfo.signalSemaphoreCount = SignalSemaphores.size();

        VkFence Fence = FencePool.Aquire();
        VK_CHECK(vkQueueSubmit(Queue, NumCommandLists, &SubmitInfo, Fence));
        VK_CHECK(vkWaitForFences(Device->GetDevice(), 1, &Fence, VK_TRUE, VULKAN_TIMEOUT_ONE_SECOND));
        FencePool.Release(Fence);

        
        WaitSemaphores.clear();
        SignalSemaphores.clear();
    }

    void FQueue::AddSignalSemaphore(VkSemaphore Semaphore)
    {
        SignalSemaphores.push_back(Semaphore);
    }

    void FQueue::AddWaitSemaphore(VkSemaphore Semaphore)
    {
        WaitSemaphores.push_back(Semaphore);
    }

    bool FVulkanStagingManager::GetStagingBuffer(TRefCountPtr<FVulkanBuffer>& OutBuffer)
    {
        if(!BufferPool.empty())
        {
            OutBuffer = BufferPool.top();
            BufferPool.pop();
            return true;
        }

        return CreateNewStagingBuffer(OutBuffer);
    }

    void FVulkanStagingManager::FreeStagingBuffer(TRefCountPtr<FVulkanBuffer> InBuffer)
    {
        VmaAllocation Allocation = Context->GetDevice()->GetAllocator()->GetAllocation(InBuffer->GetBuffer());
        void* Memory = Context->GetDevice()->GetAllocator()->MapMemory(Allocation);
        memset(Memory, 0, InBuffer->GetSize());
        Context->GetDevice()->GetAllocator()->UnmapMemory(Allocation);

        BufferPool.push(InBuffer);
    }

    void FVulkanStagingManager::FreeAllBuffers()
    {
        while(!BufferPool.empty())
        {
            BufferPool.pop();
        }
    }

    bool FVulkanStagingManager::CreateNewStagingBuffer(TRefCountPtr<FVulkanBuffer>& OutBuffer)
    {
        const uint32 vkCmdUpdateBufferLimit = Context->GetDevice()->GetPhysicalDeviceProperties().limits.maxUniformBufferRange;

        FRHIBufferDesc Desc;
        Desc.Size = vkCmdUpdateBufferLimit;
        Desc.Stride = 0;
        Desc.Usage.SetMultipleFlags(EBufferUsageFlags::StagingBuffer, EBufferUsageFlags::CPUWritable);
        FRHIBufferRef Buffer = Context->CreateBuffer(Desc);
        OutBuffer = Buffer.As<FVulkanBuffer>();

        return true;
    }

    FVulkanRenderContext::FVulkanRenderContext()
        : CurrentFrameIndex(0)
          , Queues{}
          , VulkanInstance(nullptr)
          , StagingManager(this)
          , DebugUtils()
    {
    }

    void FVulkanRenderContext::Initialize()
    {

        AssertMsg(glfwVulkanSupported(), "Vulkan Is Not Supported!");

        vkb::InstanceBuilder Builder;
        auto InstBuilder = Builder.set_app_name("Lumina Engine")
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

        CommandList = CreateCommandList({ECommandQueue::Graphics});
        
        Swapchain = FMemory::New<FVulkanSwapchain>();
        Swapchain->CreateSwapchain(VulkanInstance, this, Windowing::GetPrimaryWindowHandle(), Windowing::GetPrimaryWindowHandle()->GetExtent());
        
        WaitIdle();
        FlushPendingDeletes();
        
    }

    void FVulkanRenderContext::Deinitialize()
    {
        WaitIdle();

        StagingManager.FreeAllBuffers();
        
        FMemory::Delete(Swapchain);
        
        CommandList.SafeRelease();

        for (FQueue* Queue : Queues)
        {
            FMemory::Delete(Queue);
        }
        
        
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        func(VulkanInstance, DebugUtils.DebugMessenger, nullptr);

        
        FlushPendingDeletes();
        FMemory::Delete(VulkanDevice);
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

    void FVulkanRenderContext::FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex)
    {
        CurrentFrameIndex = InCurrentFrameIndex;
        Swapchain->AquireNextImage(CurrentFrameIndex);
        
        CommandList->Open();
        
        GetQueue(ECommandQueue::Graphics)->AddWaitSemaphore(Swapchain->GetAquireSemaphore());
        GetQueue(ECommandQueue::Graphics)->AddSignalSemaphore(Swapchain->GetPresentSemaphore());
        
    }

    void FVulkanRenderContext::FrameEnd(const FUpdateContext& UpdateContext)
    {
        FRHIImageDesc Desc;
        Desc.Dimension = EImageDimension::Texture2D;
        Desc.Extent = Swapchain->GetSwapchainExtent();
        Desc.Format = EImageFormat::R8_UNORM;
        FRHIImageRef Image = CreateImage(Desc);

        size_t TotalSize = Swapchain->GetSwapchainExtent().X * Swapchain->GetSwapchainExtent().Y * sizeof(uint8) * 4; // 4 bytes per pixel for RGBA

        // Create a buffer to hold the pixel data
        std::vector<uint8> SolidColor(TotalSize);

        // Random number generator for pixel data (RGBA values)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, 255); // Random values between 0 and 255 for each color channel

        size_t DataOffset = 0;

        // Generate random pixel data for each pixel
        for (uint32 y = 0; y < Swapchain->GetSwapchainExtent().Y; ++y)
        {
            for (uint32 x = 0; x < Swapchain->GetSwapchainExtent().X; ++x)
            {
                // Generate random color values for RGBA (each value between 0 and 255)
                uint8 R = static_cast<uint8>(dis(gen));
                uint8 G = static_cast<uint8>(dis(gen));
                uint8 B = static_cast<uint8>(dis(gen));
                uint8 A = 255;  // Full opacity

                // Pack the color values into the SolidColor array (RGBA format)
                SolidColor[DataOffset] = R;
                SolidColor[DataOffset + 1] = G;
                SolidColor[DataOffset + 2] = B;
                SolidColor[DataOffset + 3] = A;

                DataOffset += 4; // Move to the next pixel (4 bytes per pixel)
            }
        }

        // Now write the random color data to the image
        CommandList->WriteToImage(
            Image, 
            0,  // ArraySlice (for 2D textures, this is 0)
            0,  // MipLevel (typically 0 for base level)
            SolidColor.data(),  // Pointer to the pixel data
            Swapchain->GetSwapchainExtent().X * sizeof(uint8) * 4,  // Row pitch (width * bytes per pixel)
            0   // Depth pitch (0 for 2D textures)
        );


        CommandList->CopyImage(Image, Swapchain->GetCurrentImage());
        
        CommandList->SetRequiredImageAccess(Swapchain->GetCurrentImage(), ERHIAccess::PresentRead);
        CommandList->Close();
        ExecuteCommandList(CommandList);
        
        Swapchain->Present();
        
        FlushPendingDeletes();
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

        Queue->Submit(CommandLists, NumCommandLists);

        for (uint32 i = 0; i < NumCommandLists; ++i)
        {
            /** Static cast in this case if fine because CommandLists will only ever contain one type. */
            static_cast<FVulkanCommandList*>(&CommandLists[i])->Executed(Queue);
        }
    }

    FRHICommandListRef FVulkanRenderContext::GetCommandList(ECommandQueue Queue)
    {
        return CommandList;
    }

    void FVulkanRenderContext::CreateDevice(vkb::Instance Instance)
    {
        VkPhysicalDeviceVulkan13Features features = {};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features.dynamicRendering = VK_TRUE;
        features.synchronization2 = VK_TRUE;

        VkPhysicalDeviceVulkan12Features features12 = {};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features12.bufferDeviceAddress = VK_TRUE;
        features12.descriptorIndexing =  VK_TRUE;
        features12.descriptorBindingPartiallyBound = VK_TRUE;
        
        VkPhysicalDeviceFeatures device_features = {};
        device_features.samplerAnisotropy = VK_TRUE;
        device_features.sampleRateShading = VK_TRUE;
        device_features.fillModeNonSolid = VK_TRUE;
        device_features.wideLines = VK_TRUE;
        
        vkb::PhysicalDeviceSelector selector(Instance);
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features(device_features)
            .set_required_features_12(features12)
            .set_required_features_13(features)
            .require_separate_compute_queue()
            .defer_surface_initialization()
            .select()
            .value();
        

        physicalDevice.enable_extension_if_present(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
        physicalDevice.enable_extension_if_present(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        physicalDevice.enable_extension_if_present("VK_EXT_conservative_rasterization");

        vkb::DeviceBuilder deviceBuilder(physicalDevice);
        vkb::Device vkbDevice = deviceBuilder.build().value();

        
        VkDevice Device = vkbDevice.device;
        VkPhysicalDevice PhysicalDevice = physicalDevice.physical_device;
        VulkanDevice = FMemory::New<FVulkanDevice>(VulkanInstance, PhysicalDevice, Device);

        if (vkbDevice.get_queue(vkb::QueueType::graphics).has_value())
        {
            VkQueue Queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
            uint32 Index = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
            Queues[(uint32)ECommandQueue::Graphics] = FMemory::New<FQueue>(VulkanDevice, Queue, Index);
        }

        if (vkbDevice.get_queue(vkb::QueueType::compute).has_value())
        {
            VkQueue Queue = vkbDevice.get_queue(vkb::QueueType::compute).value();
            uint32 Index = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
            Queues[(uint32)ECommandQueue::Compute] = FMemory::New<FQueue>(VulkanDevice, Queue, Index);
        }

        if (vkbDevice.get_queue(vkb::QueueType::transfer).has_value())
        {
            VkQueue Queue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
            uint32 Index = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
            Queues[(uint32)ECommandQueue::Transfer] = FMemory::New<FQueue>(VulkanDevice, Queue, Index);
        }
    }

    uint64 FVulkanRenderContext::GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage)
    {
        uint64 MinAlignment = 1;

        if(Usage.AreAnyFlagsSet(EBufferUsageFlags::UniformBuffer))
        {
            MinAlignment = VulkanDevice->GetPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
        }

        return Math::GetAligned(Size, MinAlignment);
    }

    FRHIImageRef FVulkanRenderContext::CreateImage(const FRHIImageDesc& ImageSpec)
    {
        return MakeRefCount<FVulkanImage>(VulkanDevice, ImageSpec);
    }

    FRHIVertexShaderRef FVulkanRenderContext::CreateVertexShader(const TVector<uint32>& ByteCode)
    {
        return MakeRefCount<FRHIVulkanVertexShader>(VulkanDevice, ByteCode);
    }

    FRHIPixelShaderRef FVulkanRenderContext::CreatePixelShader(const TVector<uint32>& ByteCode)
    {
        return MakeRefCount<FRHIVulkanPixelShader>(VulkanDevice, ByteCode);
    }

    FRHIComputeShaderRef FVulkanRenderContext::CreateComputeShader(const TVector<uint32>& ByteCode)
    {
        return MakeRefCount<FRHIVulkanComputeShader>(VulkanDevice, ByteCode);
    }

    FRHIBufferRef FVulkanRenderContext::CreateBuffer(const FRHIBufferDesc& Description)
    {
        return MakeRefCount<FVulkanBuffer>(VulkanDevice, Description);
    }
    
    void FVulkanRenderContext::FlushPendingDeletes()
    {
        for (FQueue* Queue : Queues)
        {
            if (Queue)
            {
                Queue->RetireCommandBuffers();
            }
        }
        
        while (!PendingDeletes.empty())
        {
            IRHIResource* Resource = PendingDeletes.top();
            if (Resource->Deleting())
            {
                FMemory::Delete(Resource);
                PendingDeletes.pop();
            }
        }
    }


    void FVulkanRenderContext::SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle)
    {
        VkDebugUtilsObjectNameInfoEXT NameInfo = {};
        NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        NameInfo.objectType = ObjectType;
        NameInfo.objectHandle = Handle;
        NameInfo.pObjectName = Name.c_str();

        DebugUtils.DebugUtilsObjectNameEXT(VulkanDevice->GetDevice(), &NameInfo);
    }

    FVulkanRenderContextFunctions& FVulkanRenderContext::GetDebugUtils()
    {
        return DebugUtils;
    }
}


#endif