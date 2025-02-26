

#include "VulkanSwapchain.h"
#include "Core/Windows/Window.h"
#ifdef LUMINA_RENDERER_VULKAN

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "VulkanMacros.h"
#include "Core/Engine/Engine.h"
#include "GLFW/glfw3.h"
#include "Renderer/GPUBarrier.h"
#include "VulkanTypes.h"
#include "Core/Math/Alignment.h"
#include "Renderer/RenderHandle.h"


#include "VulkanRenderContext.h"
#include "src/VkBootstrap.h"
#include <vulkan/vulkan.hpp>

namespace Lumina
{

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
    
    VkBufferUsageFlags ToVkBufferUsage(TBitFlags<ERenderDeviceBufferUsage> Usage) 
    {
        VkBufferUsageFlags result = 0;

        if (Usage.IsFlagSet(ERenderDeviceBufferUsage::VertexBuffer))
        {
            result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
    
        if (Usage.IsFlagSet(ERenderDeviceBufferUsage::IndexBuffer))
        {
            result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
    
        if (Usage.IsFlagSet(ERenderDeviceBufferUsage::UniformBuffer))
        {
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        
        if (Usage.IsFlagSet(ERenderDeviceBufferUsage::StorageBuffer))
        {
            result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        
        return result;
    }

    constexpr VkImageLayout ToVkImageLayout(EImageLayout Layout)
    {
        switch (Layout)
        {
            case EImageLayout::Undefined:           return VK_IMAGE_LAYOUT_UNDEFINED;
            case EImageLayout::General:             return VK_IMAGE_LAYOUT_GENERAL;
            case EImageLayout::ColorAttachment:     return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case EImageLayout::DepthAttachment:     return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case EImageLayout::DepthReadOnly:       return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case EImageLayout::ShaderReadOnly:      return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case EImageLayout::TransferSource:      return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case EImageLayout::TransferDestination: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case EImageLayout::PresentSource:       return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            case EImageLayout::Default:             return VK_IMAGE_LAYOUT_UNDEFINED;
            default:                                return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }



    FVulkanMemoryAllocator::FVulkanMemoryAllocator(VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkDevice Device)
    {
        VmaVulkanFunctions Functions = {};
        Functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        Functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo Info = {};
        Info.vulkanApiVersion = VK_API_VERSION_1_3;
        Info.instance = Instance;
        Info.physicalDevice = PhysicalDevice;
        Info.device = Device;
        Info.pVulkanFunctions = &Functions;

        VK_CHECK(vmaCreateAllocator(&Info, &Allocator));

        Statistics = {};
    }

    FVulkanMemoryAllocator::~FVulkanMemoryAllocator()
    {
    }

    void FVulkanMemoryAllocator::ClearAllAllocations()
    {
        // Cleanup allocated buffers
        for (auto& kvp : Statistics.AllocatedBuffers)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Buffer Object with VkBuffer: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyBuffer(Allocator, kvp.first, kvp.second);
            }
        }
        Statistics.AllocatedBuffers.clear();

        // Cleanup allocated images
        for (auto& kvp : Statistics.AllocatedImages)
        {
            if (kvp.first != VK_NULL_HANDLE)
            {
                LOG_DEBUG("Destroying Image Object with VkImage: {}, VmaAllocation: {}", (void*)kvp.first, (void*)kvp.second);
                vmaDestroyImage(Allocator, kvp.first, kvp.second);
            }
        }
        
        Statistics.AllocatedImages.clear();

        // Destroy the Vulkan memory allocator
        LOG_INFO("Destroying Vulkan Memory Allocator...");
        vmaDestroyAllocator(Allocator);
        Allocator = VK_NULL_HANDLE;

        // Reset statistics
        Statistics.CurrentlyAllocatedBuffers = 0;
        Statistics.CurrentlyAllocatedImages = 0;
        Statistics.CurrentlyAllocated = 0;

        // Log final state
        LOG_INFO("Allocator Shutdown Complete. Final Statistics:");
        LOG_INFO("Allocated Buffers: {}", Statistics.CurrentlyAllocatedBuffers);
        LOG_INFO("Allocated Images: {}", Statistics.CurrentlyAllocatedImages);
        LOG_INFO("Currently Allocated Memory: {} bytes", Statistics.CurrentlyAllocated);
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateBuffer(VkBufferCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkBuffer* vkBuffer, const char* AllocationName)
    {
        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation = nullptr;
        VmaAllocationInfo AllocationInfo;

        VK_CHECK(vmaCreateBuffer(Allocator, CreateInfo, &Info, vkBuffer, &Allocation, &AllocationInfo));
        AssertMsg(Allocation, "Vulkan failed to allocate buffer memory!");

        vmaSetAllocationName(Allocator, Allocation, AllocationName);

        Statistics.Allocations.push_back(Allocation);
        Statistics.AllocatedBuffers[*vkBuffer]= Allocation;
        Statistics.Allocated += AllocationInfo.size;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        Statistics.CurrentlyAllocatedBuffers++;
        
        return Allocation;
    }

    VmaAllocation FVulkanMemoryAllocator::AllocateImage(VkImageCreateInfo* CreateInfo, VmaAllocationCreateFlags Flags, VkImage* vkImage, const char* AllocationName)
    {
        if (CreateInfo->extent.depth == 0)
        {
            LOG_WARN("Trying to allocate image with 0 depth. No allocation done");
            return VK_NULL_HANDLE;
        }

        VmaAllocationCreateInfo Info = {};
        Info.usage = VMA_MEMORY_USAGE_AUTO;
        Info.flags = Flags;

        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;

        VK_CHECK(vmaCreateImage(Allocator, CreateInfo, &Info, vkImage, &Allocation, &AllocationInfo));
        AssertMsg(Allocation, "Vulkan failed to allocate image memory!");

        
        vmaSetAllocationName(Allocator, Allocation, AllocationName);

        Statistics.Allocated += AllocationInfo.size;
        Statistics.AllocatedImages[*vkImage] = Allocation;
        Statistics.CurrentlyAllocated += AllocationInfo.size;
        Statistics.CurrentlyAllocatedImages++;
        
        return Allocation;
    }



    void FVulkanMemoryAllocator::DestroyBuffer(VkBuffer Buffer, VmaAllocation Allocation)
    {
        if (Buffer && Allocation)
        {
            
            
            VmaAllocationInfo AllocationInfo;
            vmaGetAllocationInfo(Allocator, Allocation, &AllocationInfo);
            
            Statistics.CurrentlyAllocated -= AllocationInfo.size;
            Statistics.CurrentlyAllocatedBuffers--;


            vmaDestroyBuffer(Allocator, Buffer, Allocation);
            
            Statistics.AllocatedBuffers.erase(Buffer);
            
            Allocation = VK_NULL_HANDLE;
        }
    }


    void FVulkanMemoryAllocator::DestroyImage(VkImage Image, VmaAllocation Allocation)
    {
        if (Image && Allocation)
        {
            VmaAllocationInfo AllocationInfo;
            vmaGetAllocationInfo(Allocator, Allocation, &AllocationInfo);
            
            Statistics.CurrentlyAllocated -= AllocationInfo.size;
            Statistics.CurrentlyAllocatedImages--;
            
            vmaDestroyImage(Allocator, Image, Allocation);
            
            Statistics.AllocatedImages.erase(Image);

            Allocation = VK_NULL_HANDLE;
        }
    }

    void* FVulkanMemoryAllocator::MapMemory(VmaAllocation Allocation)
    {
        void* MappedMemory;
        VK_CHECK(vmaMapMemory(Allocator, Allocation, &MappedMemory));
        return MappedMemory;
    }

    void FVulkanMemoryAllocator::UnmapMemory(VmaAllocation Allocation)
    {
        vmaUnmapMemory(Allocator, Allocation);
    }
    
    

    //------------------------------------------------------------------------------------

    

    // We create one command pool per thread.
    struct FCommandPools
    {
        TVector<VkCommandPool> CommandPools;
        TVector<VkCommandPool> TransientCommandPools;
    } CommandPools;
    
    FVulkanRenderContext::FVulkanRenderContext()
    {
        Device = VK_NULL_HANDLE;
        PhysicalDevice = VK_NULL_HANDLE;
    }

    struct FStagingBufferPool
    {
        TVector<VkBuffer> AvailableBuffers;
    } StagingBufferPools;

    void FVulkanRenderContext::Initialize()
    {
        MemoryAllocator = FMemory::New<FVulkanMemoryAllocator>();

        AssertMsg(glfwVulkanSupported(), "Vulkan Is Not Supported!");

        vkb::InstanceBuilder Builder;
        auto InstBuilder = Builder.set_app_name("Lumina Engine")
        .request_validation_layers()
        .use_default_debug_messenger()
        .set_debug_callback(VkDebugCallback)
        .enable_extension("VK_EXT_debug_utils")
        .require_api_version(1, 3, 0)
        .build();

        VulkanInstance = InstBuilder.value();
        
        DebugUtils.DebugMessenger = InstBuilder->debug_messenger;
        DebugUtils.DebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(vkGetInstanceProcAddr(
            VulkanInstance, "vkSetDebugUtilsObjectNameEXT"));
        
        
        CreateDevice(InstBuilder.value());
        
        Swapchain = FMemory::New<FVulkanSwapchain>();
        Swapchain->CreateSwapchain(VulkanInstance, Device, Windowing::GetPrimaryWindowHandle(), Windowing::GetPrimaryWindowHandle()->GetExtent());
    }

    void FVulkanRenderContext::Deinitialize()
    {
        
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
        

        physicalDevice.enable_extension_if_present("VK_KHR_dynamic_rendering");
        physicalDevice.enable_extension_if_present("VK_EXT_conservative_rasterization");

        vkb::DeviceBuilder deviceBuilder(physicalDevice);
        vkb::Device vkbDevice = deviceBuilder.build().value();
        
        CommandQueues.GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        CommandQueues.TransferQueue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
        CommandQueues.ComputeQueue = vkbDevice.get_queue(vkb::QueueType::compute).value();
        
        CommandQueues.GraphicsQueueIndex =     vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        CommandQueues.ComputeQueueIndex =      vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        CommandQueues.TransferQueueIndex =     vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        
        Device = vkbDevice.device;
        PhysicalDevice = physicalDevice.physical_device;


        BufferPool.SetFreeCallback([](FVulkanBuffer* Buffer)
        {
            
        });

        ImagePool.SetFreeCallback([](FVulkanImage* Image)
        {
            
        });

        for (int i = 0; i < 100; ++i)
        {
            VkBufferCreateInfo BufferCreateInfo = {};
            BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            BufferCreateInfo.size = 0;
            BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            BufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            BufferCreateInfo.flags = 0;

            VmaAllocationCreateFlags VmaFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            
            VkBuffer VkBuffer;
            MemoryAllocator->AllocateBuffer(&BufferCreateInfo, VmaFlags, &VkBuffer, "Staging Buffer");
            StagingBufferPools.AvailableBuffers.push_back(VkBuffer);
        }
        
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
        vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

        for(uint32 i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            VkCommandPoolCreateFlags Flags;
            
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

    uint64 FVulkanRenderContext::GetAlignedSizeForBuffer(uint64 Size, TBitFlags<ERenderDeviceBufferUsage> Usage)
    {
        uint64 MinAlignment = 1;

        if(Usage.AreAnyFlagsSet(ERenderDeviceBufferUsage::UniformBuffer))
        {
            MinAlignment = PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
        }
        if(Usage.AreAnyFlagsSet(ERenderDeviceBufferUsage::StorageBuffer))
        {
            MinAlignment = PhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
        }

        return Math::GetAligned(Size, MinAlignment);
    }

    FRHIImageHandle FVulkanRenderContext::CreateTexture(FVector2D Extent)
    {
        return {};
    }

    FRHIImageHandle FVulkanRenderContext::CreateRenderTarget(FVector2D Extent)
    {
        return {};
    }

    FRHIImageHandle FVulkanRenderContext::CreateDepthImage(FVector2D Extent)
    {
        return {};
    }

    void FVulkanRenderContext::Barrier(FGPUBarrier* Barriers, uint32 BarrierNum, FCommandList* CommandList)
    {
        Assert(Barriers != nullptr);
        
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;
        VkCommandBuffer CommandBuffer = VulkanCommandList->CommandBuffer;
        
        VkImageMemoryBarrier imageBarriers[16];
        uint32 numImageBarriers = 0;
    
        for (uint32 i = 0; i < BarrierNum; ++i)
        {
            FGPUBarrier* Barrier = &Barriers[i];
    
            switch (Barrier->Type)
            {
            case FGPUBarrier::EType::Image:
            {

                FVulkanImage* VulkanImage = ImagePool.GetResource(Barrier->RHIImage);
                    
                VkImageMemoryBarrier& imageMemBarrier = imageBarriers[numImageBarriers++];
                imageMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageMemBarrier.pNext = nullptr;
                imageMemBarrier.srcAccessMask = 0;
                imageMemBarrier.dstAccessMask = 0;
                imageMemBarrier.oldLayout = ToVkImageLayout(Barriers->FromLayout);
                imageMemBarrier.newLayout = (Barrier->ToLayout == EImageLayout::Default) ? ToVkImageLayout(VulkanImage->Usage) : ToVkImageLayout(Barrier->ToLayout);
                imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemBarrier.image = VulkanImage->Image;
                imageMemBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }; 
    
                break;
            }
    
            case FGPUBarrier::EType::Memory:
            {
                break;
            }
            }
        }
    
        if (numImageBarriers > 0)
        {
            vkCmdPipelineBarrier(
                CommandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0,
                0, nullptr,
                0, nullptr,
                numImageBarriers, imageBarriers
            );
        }
    }

    

    FRHIBufferHandle FVulkanRenderContext::CreateBuffer(TBitFlags<ERenderDeviceBufferUsage> UsageFlags, TBitFlags<ERenderDeviceBufferMemoryUsage> MemoryUsage, uint32 Size)
    {
        VmaAllocationCreateFlags VmaFlags = 0;
        
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = GetAlignedSizeForBuffer(Size, UsageFlags);
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = ToVkBufferUsage(UsageFlags);
        BufferCreateInfo.flags = 0;
        
        VkBuffer VkBuffer;
        VmaAllocation Allocation = MemoryAllocator->AllocateBuffer(&BufferCreateInfo, VmaFlags, &VkBuffer, "");

        FRHIBufferHandle BufferHandle = BufferPool.Allocate();
        FVulkanBuffer* Buffer = BufferPool.GetResource(BufferHandle);

        Buffer->Allocation = Allocation;
        Buffer->Buffer = VkBuffer;

        return BufferHandle;
    }

    void FVulkanRenderContext::UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset)
    {
        VmaAllocationCreateFlags VmaFlags = 0;
        
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = Size;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        BufferCreateInfo.flags = 0;
        
        VkBuffer VkStagingBuffer;
        VmaAllocation Allocation = MemoryAllocator->AllocateBuffer(&BufferCreateInfo, VmaFlags, &VkStagingBuffer, "Staging Buffer");
        
        void* Memory = MemoryAllocator->MapMemory(Allocation);
        FMemory::MemCopy((char*)Memory + Offset, Data, Size);
        MemoryAllocator->UnmapMemory(Allocation);
        
        FRHIBufferHandle StagingBufferHandle = BufferPool.Allocate();
        FVulkanBuffer* StagingBuffer = BufferPool.GetResource(StagingBufferHandle);
        StagingBuffer->Allocation = Allocation;
        StagingBuffer->Buffer = VkStagingBuffer;
        StagingBuffer->Size = Size;
        
        FVulkanBuffer* VulkanBuffer = BufferPool.GetResource(Buffer);

        CopyBuffer(VulkanBuffer, StagingBuffer);
        
    }

    void FVulkanRenderContext::CopyBuffer(FRHIBufferHandle Source, FRHIBufferHandle Destination)
    {
        FVulkanBuffer* SourceBuffer = BufferPool.GetResource(Source);
        FVulkanBuffer* DestinationBuffer = BufferPool.GetResource(Destination);
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = CommandPools.TransientCommandPools[0];
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VK_CHECK(vkAllocateCommandBuffers(Device, &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = SourceBuffer->Size;
        
        vkCmdCopyBuffer(commandBuffer, SourceBuffer->Buffer, DestinationBuffer->Buffer, 1, &copyRegion);

        VK_CHECK(vkEndCommandBuffer(commandBuffer));

    }

    
    FCommandList* FVulkanRenderContext::BeginCommandList(ECommandQueue CommandType, ECommandBufferUsage Usage)
    {

        bool bTransient = (Usage == ECommandBufferUsage::Transient);
        
        VkCommandBufferAllocateInfo AllocInfo = {};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = bTransient ? CommandPools.TransientCommandPools[0] : CommandPools.CommandPools[0];
        AllocInfo.level = bTransient ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        FVulkanCommandList* VulkanCommandList = FMemory::New<FVulkanCommandList>();
        
        VK_CHECK(vkAllocateCommandBuffers(Device, &AllocInfo, &VulkanCommandList->CommandBuffer));
        
        CommandList.push_back(VulkanCommandList);

        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = bTransient ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_NO_FLAGS;
        
        VK_CHECK(vkBeginCommandBuffer(VulkanCommandList->CommandBuffer, &BeginInfo));

        return VulkanCommandList;
    }

    void FVulkanRenderContext::EndCommandList(FCommandList* CommandList, bool bDestroy)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;

        VK_CHECK(vkEndCommandBuffer(VulkanCommandList->CommandBuffer));

        if (bDestroy)
        {
            VkCommandPool Pool = CommandList->Type == ECommandBufferUsage::Transient ? CommandPools.TransientCommandPools[0] : CommandPools.CommandPools[0];
            vkFreeCommandBuffers(Device, Pool, 1, &VulkanCommandList->CommandBuffer);
            
            FMemory::Free(CommandList);
        }
    }

    void FVulkanRenderContext::BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo)
    {
        TVector<VkRenderingAttachmentInfo> ColorAttachments;
        VkRenderingAttachmentInfo DepthAttachment = {};

        for (int i = 0; i < PassInfo.Attachments.size(); ++i)
        {
            const FRHIImageHandle& ImageHandle = PassInfo.Attachments[i];

            FVulkanImage* Image = ImagePool.GetResource(ImageHandle);
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = Image->ImageView;
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.LoadOps[i] == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp =VK_ATTACHMENT_STORE_OP_STORE;
    
            if (Image->Usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            {
                Attachment.clearValue.color.float32[0] = 1.0f;
                Attachment.clearValue.color.float32[1] = 1.0f;
                Attachment.clearValue.color.float32[2] = 1.0f;
                Attachment.clearValue.color.float32[3] = 1.0f;
    
                ColorAttachments.push_back(Attachment);
            }
            else if (Image->Usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                DepthAttachment = Attachment;
                DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                DepthAttachment.clearValue.depthStencil.depth = 1.0f;
                DepthAttachment.clearValue.depthStencil.stencil = 0;
            }
        }
        
        VkRenderingInfo RenderInfo = {};
        RenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        RenderInfo.colorAttachmentCount = (uint32)ColorAttachments.size();
        RenderInfo.pColorAttachments = ColorAttachments.data();
        RenderInfo.pDepthAttachment = (DepthAttachment.imageView != VK_NULL_HANDLE) ? &DepthAttachment : nullptr;
        RenderInfo.renderArea.extent.width = GEngine->GetEngineViewport().GetSize().X;
        RenderInfo.renderArea.extent.height = GEngine->GetEngineViewport().GetSize().Y;
        RenderInfo.layerCount = 1;

        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;
        
        vkCmdBeginRendering(VulkanCommandList->CommandBuffer, &RenderInfo);
    }

    void FVulkanRenderContext::EndRenderPass(FCommandList* CommandList)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;

        vkCmdEndRendering(VulkanCommandList->CommandBuffer);
        
    }
}


#endif