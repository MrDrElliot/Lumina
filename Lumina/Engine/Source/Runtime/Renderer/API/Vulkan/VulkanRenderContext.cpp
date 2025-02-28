

#include "VulkanCommandList.h"
#include "VulkanDevice.h"
#include "Renderer/CommandList.h"
#ifdef LUMINA_RENDERER_VULKAN

#include "VulkanResources.h"
#include "VulkanSwapchain.h"
#include "Core/Windows/Window.h"
#include "Renderer/GPUBarrier.h"


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

    struct FVulkanRenderContextFunctions
    {
        VkDebugUtilsMessengerEXT DebugMessenger;
        PFN_vkSetDebugUtilsObjectNameEXT DebugUtilsObjectNameEXT;
    } DebugUtils;
    
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

    

    // We create one command pool per thread.
    namespace
    {
        struct FCommandPools
        {
            TVector<VkCommandPool> CommandPools;
            TVector<VkCommandPool> TransientCommandPools;
        } CommandPools;
    }
    

    FVulkanRenderContext::FVulkanRenderContext()
        : CurrentFrameIndex(0)
        , PrimaryCommandList{}
        , Swapchain(nullptr)
        , VulkanInstance(nullptr)
        , CommandQueues()
        , VulkanDevice(nullptr)
        , FencePool()
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
        
        FencePool.SetDevice(VulkanDevice->GetDevice());
        
        Swapchain = FMemory::New<FVulkanSwapchain>();
        Swapchain->CreateSwapchain(VulkanInstance, this, Windowing::GetPrimaryWindowHandle(), Windowing::GetPrimaryWindowHandle()->GetExtent());
        
        for (size_t i = 0; i < std::size(PrimaryCommandList); ++i)
        {
            PrimaryCommandList[i] = (FVulkanCommandList*)AllocateCommandList(ECommandBufferLevel::Primary);
        }
    }

    void FVulkanRenderContext::Deinitialize()
    {
        WaitIdle();
        
        FMemory::Delete(Swapchain);
        
        FencePool.Destroy();

        while (!CommandQueue.empty())
        {
            FVulkanCommandList* CommandList = CommandQueue.back();

            vkDestroyFence(VulkanDevice->GetDevice(), CommandList->Fence, nullptr);
            FMemory::Delete(CommandList);
            
            CommandQueue.pop();
        }

        FlushPendingDeletes();

        for (FVulkanCommandList* List : PrimaryCommandList)
        {
            FMemory::Delete(List);
        }
        
        for (VkCommandPool CommandPool : CommandPools.CommandPools)
        {
            vkDestroyCommandPool(VulkanDevice->GetDevice(), CommandPool, nullptr);
        }

        for (VkCommandPool CommandPool : CommandPools.TransientCommandPools)
        {
            vkDestroyCommandPool(VulkanDevice->GetDevice(), CommandPool, nullptr);
        }

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        func(VulkanInstance, DebugUtils.DebugMessenger, nullptr);

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

        
        // Begin primary command list.
        GetPrimaryCommandList()->Begin();
        GetPrimaryCommandList()->WaitSemaphores.push_back(Swapchain->GetAquireSemaphore());
        GetPrimaryCommandList()->SignalSemaphores.push_back(Swapchain->GetPresentSemaphore());
        
    }

    void FVulkanRenderContext::FrameEnd(const FUpdateContext& UpdateContext)
    {
        TVector<VkCommandBuffer> SecondaryCommandBuffers;
        FVulkanCommandList* PrimaryList = GetPrimaryCommandList();
        
        PrimaryList->FlushCommandList();
        PrimaryList->SubmitCommandList();
        PrimaryList->Reset();
        
        Swapchain->Present();

        FlushPendingDeletes();
        
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
        

        physicalDevice.enable_extension_if_present(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        physicalDevice.enable_extension_if_present("VK_EXT_conservative_rasterization");

        vkb::DeviceBuilder deviceBuilder(physicalDevice);
        vkb::Device vkbDevice = deviceBuilder.build().value();
        
        CommandQueues.GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        CommandQueues.TransferQueue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
        CommandQueues.ComputeQueue = vkbDevice.get_queue(vkb::QueueType::compute).value();
        
        CommandQueues.GraphicsQueueIndex =     vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        CommandQueues.ComputeQueueIndex =      vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        CommandQueues.TransferQueueIndex =     vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        
        VkDevice Device = vkbDevice.device;
        VkPhysicalDevice PhysicalDevice = physicalDevice.physical_device;

        VulkanDevice = FMemory::New<FVulkanDevice>(VulkanInstance, PhysicalDevice, Device);

        for(uint32 i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            VkCommandPoolCreateFlags Flags = 0;
            Flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            
            VkCommandPoolCreateInfo CreateInfo = {};
            CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            CreateInfo.flags = Flags;

            {
                VkCommandPool Pool;
                VK_CHECK(vkCreateCommandPool(Device, &CreateInfo, nullptr, &Pool));
                CommandPools.CommandPools.push_back(Pool);
            }

            
            {
                Flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
                
                VkCommandPool Pool;
                VK_CHECK(vkCreateCommandPool(Device, &CreateInfo, nullptr, &Pool));
                CommandPools.TransientCommandPools.push_back(Pool);
            }
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

    FRHIVertexShaderRef FVulkanRenderContext::CreateVertexShader(const TVector<const uint32>& ByteCode)
    {
        return MakeRefCount<FRHIVulkanVertexShader>(VulkanDevice, ByteCode);
    }

    FRHIPixelShaderRef FVulkanRenderContext::CreatePixelShader(const TVector<const uint32>& ByteCode)
    {
        return MakeRefCount<FRHIVulkanPixelShader>(VulkanDevice, ByteCode);
    }

    FRHIComputeShaderRef FVulkanRenderContext::CreateComputeShader(const TVector<const uint32>& ByteCode)
    {
        return MakeRefCount<FRHIVulkanComputeShader>(VulkanDevice, ByteCode);
    }

    FRHIBufferRef FVulkanRenderContext::CreateBuffer(const FRHIBufferDesc& Description)
    {
        return MakeRefCount<FVulkanBuffer>(VulkanDevice, Description);
    }

    void FVulkanRenderContext::UploadToBuffer(ICommandList* CommandList, FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size)
    {
        VmaAllocationCreateFlags VmaFlags = 0;
        
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = Size;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        BufferCreateInfo.flags = 0;


        TBitFlags<EBufferUsageFlags> BufferUsage;
        BufferUsage.SetFlag(EBufferUsageFlags::StagingBuffer);
        
        FRHIBufferDesc Description;
        Description.Size = Size;
        Description.Stride = Size;
        Description.Usage = BufferUsage;

        FRHIBufferRef StagingBuffer = CreateBuffer(Description);
        VmaAllocation Allocation = VulkanDevice->GetAllocator()->GetAllocation(StagingBuffer.As<FVulkanBuffer>()->GetBuffer());
        
        void* Memory = VulkanDevice->GetAllocator()->MapMemory(Allocation);
        FMemory::MemCopy((char*)Memory + Offset, Data, Size);
        VulkanDevice->GetAllocator()->UnmapMemory(Allocation);

        CopyBuffer(CommandList, StagingBuffer, Buffer);
        
    }

    void FVulkanRenderContext::CopyBuffer(ICommandList* CommandList, FRHIBuffer* Source, FRHIBuffer* Destination)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = CommandPools.TransientCommandPools[0];
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VK_CHECK(vkAllocateCommandBuffers(VulkanDevice->GetDevice(), &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = Source->GetSize();

        FVulkanBuffer* VkSource = static_cast<FVulkanBuffer*>(Source);
        FVulkanBuffer* VkDestination = static_cast<FVulkanBuffer*>(Destination);
        
        vkCmdCopyBuffer(commandBuffer, VkSource->GetBuffer(), VkDestination->GetBuffer(), 1, &copyRegion);

        VK_CHECK(vkEndCommandBuffer(commandBuffer));

    }

    FVulkanCommandList* FVulkanRenderContext::GetPrimaryCommandList() const
    {
        return PrimaryCommandList[CurrentFrameIndex];
    }
    
    ICommandList* FVulkanRenderContext::AllocateCommandList(ECommandBufferLevel Level, ECommandQueue CommandType, ECommandBufferUsage Usage)
    {
        bool bTransient = (Usage == ECommandBufferUsage::Transient);
        
        VkCommandBufferAllocateInfo AllocInfo = {};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = bTransient ? CommandPools.TransientCommandPools[0] : CommandPools.CommandPools[0];
        AllocInfo.level = (Level == ECommandBufferLevel::Primary) ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        AllocInfo.commandBufferCount = 1;


        VkQueue Queue = VK_NULL_HANDLE;
        switch (CommandType)
        {
        case ECommandQueue::Graphics:
            Queue = CommandQueues.GraphicsQueue;
            break;
        case ECommandQueue::Compute:
            Queue = CommandQueues.ComputeQueue;
            break;
        case ECommandQueue::Transfer:
            Queue = CommandQueues.TransferQueue;
            break;
        case ECommandQueue::All:
            Queue = CommandQueues.GraphicsQueue;
            break;
        }
        
        FVulkanCommandList* VulkanCommandList = FMemory::New<FVulkanCommandList>();
        VulkanCommandList->CommandQueue = CommandType;
        VulkanCommandList->Queue = Queue;
        VulkanCommandList->Type = Usage;
        VulkanCommandList->Pool = AllocInfo.commandPool;
        VulkanCommandList->RenderContext = this;

        /** Primary command buffer is managed seperately */
        if (Level == ECommandBufferLevel::Secondary && Usage != ECommandBufferUsage::Transient)
        {
            CommandQueue.push(VulkanCommandList);
        }
        
        VK_CHECK(vkAllocateCommandBuffers(GetDevice()->GetDevice(), &AllocInfo, &VulkanCommandList->CommandBuffer));

        return VulkanCommandList;
    }
    
    void FVulkanRenderContext::BeginRenderPass(ICommandList* CommandList, const FRenderPassBeginInfo& PassInfo)
    {
        TVector<VkRenderingAttachmentInfo> ColorAttachments;
        VkRenderingAttachmentInfo DepthAttachment = {};

        for (int i = 0; i < PassInfo.ColorAttachments.size(); ++i)
        {
            FRHIImageRef Image = PassInfo.ColorAttachments[i];

            TRefCountPtr<FVulkanImage> VulkanImage = Image.As<FVulkanImage>();
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.ColorLoadOps[i] == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.ColorStoreOps[i] == ERenderLoadOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE; 
    
            Attachment.clearValue.color.float32[0] = PassInfo.ClearColorValues[i].R;
            Attachment.clearValue.color.float32[1] = PassInfo.ClearColorValues[i].G;
            Attachment.clearValue.color.float32[2] = PassInfo.ClearColorValues[i].B;
            Attachment.clearValue.color.float32[3] = PassInfo.ClearColorValues[i].A;
    
            ColorAttachments.push_back(Attachment);
        }
        
        const FRHIImageRef& ImageHandle = PassInfo.DepthAttachment;
        if (ImageHandle.IsValid())
        {
            TRefCountPtr<FVulkanImage> VulkanImage = PassInfo.DepthAttachment.As<FVulkanImage>();
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.DepthLoadOp == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.DepthStoreOp == ERenderLoadOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
            
            DepthAttachment = Attachment;
            DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            DepthAttachment.clearValue.depthStencil.depth = 1.0f;
            DepthAttachment.clearValue.depthStencil.stencil = 0;
            
        }
        
        VkRenderingInfo RenderInfo = {};
        RenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        RenderInfo.colorAttachmentCount = (uint32)ColorAttachments.size();
        RenderInfo.pColorAttachments = ColorAttachments.data();
        RenderInfo.pDepthAttachment = (DepthAttachment.imageView != VK_NULL_HANDLE) ? &DepthAttachment : nullptr;
        RenderInfo.renderArea.extent.width = PassInfo.RenderArea.X;
        RenderInfo.renderArea.extent.height = PassInfo.RenderArea.Y;
        RenderInfo.layerCount = 1;

        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;
        
        vkCmdBeginRendering(VulkanCommandList->CommandBuffer, &RenderInfo);
    }

    void FVulkanRenderContext::EndRenderPass(ICommandList* CommandList)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;

        vkCmdEndRendering(VulkanCommandList->CommandBuffer);
        
    }

    void FVulkanRenderContext::ClearColor(ICommandList* CommandList, const FColor& Color)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;
        
    }

    void FVulkanRenderContext::Draw(ICommandList* CommandList, uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance)
    {
        FVulkanCommandList* VulknanCommandList = (FVulkanCommandList*)CommandList;

        vkCmdDraw(VulknanCommandList->CommandBuffer, VertexCount, InstanceCount, FirstVertex, FirstInstance);
    }

    void FVulkanRenderContext::DrawIndexed(ICommandList* CommandList, uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance)
    {
        FVulkanCommandList* VulknanCommandList = (FVulkanCommandList*)CommandList;

        vkCmdDrawIndexed(VulknanCommandList->CommandBuffer, IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
    }

    void FVulkanRenderContext::Dispatch(ICommandList* CommandList, uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ)
    {
        FVulkanCommandList* VulknanCommandList = (FVulkanCommandList*)CommandList;

        vkCmdDispatch(VulknanCommandList->CommandBuffer, GroupCountX, GroupCountY, GroupCountZ);
    }

    void FVulkanRenderContext::FlushPendingDeletes()
    {
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
    
}


#endif