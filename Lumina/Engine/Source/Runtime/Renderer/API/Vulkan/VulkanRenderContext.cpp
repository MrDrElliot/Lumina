

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

    VkAccessFlags GetImageAccessMask(EImageLayout layout)
    {
        switch (layout)
        {
            case EImageLayout::General:                 return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            case EImageLayout::TransferDestination:     return VK_ACCESS_TRANSFER_WRITE_BIT;
            case EImageLayout::TransferSource:          return VK_ACCESS_TRANSFER_READ_BIT;
            case EImageLayout::ColorAttachment:         return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            case EImageLayout::DepthAttachment:         return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            case EImageLayout::PresentSource:           return VK_ACCESS_MEMORY_READ_BIT;
            default:
                return 0;
        }
    }

    EImageLayout FromVkImageLayout(VkImageLayout Layout)
    {
        switch (Layout)
        {
            case VK_IMAGE_LAYOUT_GENERAL:                           return EImageLayout::General;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:              return EImageLayout::TransferDestination;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:              return EImageLayout::TransferSource;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:          return EImageLayout::ColorAttachment;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:  return EImageLayout::DepthAttachment;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                   return EImageLayout::PresentSource;
            default:                                                return EImageLayout::Default;
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
    static struct FCommandPools
    {
        TVector<VkCommandPool> CommandPools;
        TVector<VkCommandPool> TransientCommandPools;
    } CommandPools;

    
    FVulkanRenderContext::FVulkanRenderContext()
        : BufferPool()
        , ImagePool()
        , Swapchain(nullptr)
        , VulkanInstance(nullptr)
        , CommandQueues()
        , PhysicalDeviceProperties()
        , PhysicalDeviceMemoryProperties()
    {
        Device = VK_NULL_HANDLE;
        PhysicalDevice = VK_NULL_HANDLE;
    }

    static struct FStagingBufferPool
    {
        TVector<VkBuffer> AvailableBuffers;
    } StagingBufferPools;

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
        
        MemoryAllocator = FMemory::New<FVulkanMemoryAllocator>(VulkanInstance, PhysicalDevice, Device);
        FencePool.SetDevice(Device);
        
        Swapchain = FMemory::New<FVulkanSwapchain>();
        Swapchain->CreateSwapchain(VulkanInstance, this, Windowing::GetPrimaryWindowHandle(), Windowing::GetPrimaryWindowHandle()->GetExtent());
        
        /*for (int i = 0; i < 100; ++i)
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
        }*/
    }

    void FVulkanRenderContext::Deinitialize()
    {
        WaitIdle();
        
        FMemory::Delete(Swapchain);
        
        MemoryAllocator->ClearAllAllocations();
        FMemory::Delete(MemoryAllocator);
        
        FencePool.Destroy();

        LOG_INFO("Num Command Lists: {0}", CommandQueue.size());
        while (!CommandQueue.empty())
        {
            FVulkanCommandList* CommandList = CommandQueue.back();

            vkDestroyFence(Device, CommandList->Fence, nullptr);
            FMemory::Delete(CommandList);
            
            CommandQueue.pop();
        }
        
        for (VkCommandPool CommandPool : CommandPools.CommandPools)
        {
            vkDestroyCommandPool(Device, CommandPool, nullptr);
        }

        for (VkCommandPool CommandPool : CommandPools.TransientCommandPools)
        {
            vkDestroyCommandPool(Device, CommandPool, nullptr);
        }

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        func(VulkanInstance, DebugUtils.DebugMessenger, nullptr);
        
        vkDestroyDevice(Device, nullptr);
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
        VK_CHECK(vkDeviceWaitIdle(Device));
    }

    void FVulkanRenderContext::FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex)
    {
        CurrentFrameIndex = InCurrentFrameIndex;
        
        // Begin primary command list.
        PrimaryCommandList[CurrentFrameIndex] = (FVulkanCommandList*)(BeginCommandList(ECommandBufferLevel::Primary));
        
        Swapchain->AquireNextImage(CurrentFrameIndex);
        
        PrimaryCommandList[CurrentFrameIndex]->WaitSemaphores.push_back(Swapchain->GetAquireSemaphore());
        PrimaryCommandList[CurrentFrameIndex]->SignalSemaphores.push_back(Swapchain->GetPresentSemaphore());
        
    }

    void FVulkanRenderContext::FrameEnd(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex)
    {
        TVector<VkCommandBuffer> SecondaryCommandBuffers;
        FVulkanCommandList* PrimaryList = PrimaryCommandList[CurrentFrameIndex];

        while (!CommandQueue.empty())
        {
            FVulkanCommandList* CommandList = CommandQueue.back();
            EndCommandList(CommandList);
            CommandQueue.pop();
        }
        
        if (!SecondaryCommandBuffers.empty())
        {
            vkCmdExecuteCommands(PrimaryList->CommandBuffer, SecondaryCommandBuffers.size(), SecondaryCommandBuffers.data());
        }

        EndCommandList(PrimaryList);
        
        Swapchain->Present();
        
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


        BufferPool.SetFreeCallback([this](FVulkanBuffer* Buffer)
        {
            MemoryAllocator->DestroyBuffer(Buffer->Buffer, Buffer->Allocation);
        });

        ImagePool.SetFreeCallback([this](FVulkanImage* Image)
        {
            MemoryAllocator->DestroyImage(Image->Image, Image->Allocation);
            vkDestroyImageView(Device, Image->ImageView, nullptr);
        });
        
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
        vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

        for(uint32 i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            VkCommandPoolCreateFlags Flags = 0;
            
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

    FRHIImageHandle FVulkanRenderContext::AllocateImage()
    {
        return ImagePool.Allocate();
    }

    FRHIImageHandle FVulkanRenderContext::CreateTexture(const FImageSpecification& ImageSpec)
    {
        VkImageCreateFlags ImageFlags = VK_NO_FLAGS;
        
        VkImageCreateInfo ImageCreateInfo = {};
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageCreateInfo.extent = VkExtent3D(ImageSpec.Extent.X, ImageSpec.Extent.Y, 1);
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.flags = ImageFlags;
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.mipLevels = 1;
        ImageCreateInfo.arrayLayers = 1;
        ImageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        FRHIImageHandle Handle = ImagePool.Allocate();
        FVulkanImage* VulkanImage =ImagePool.GetResource(Handle);

        VmaAllocationCreateFlags AllocationFlags = VK_NO_FLAGS;
        
        MemoryAllocator->AllocateImage(&ImageCreateInfo, AllocationFlags, &VulkanImage->Image, "");

        return Handle;
    }

    FRHIImageHandle FVulkanRenderContext::CreateRenderTarget(const FIntVector2D& Extent)
    {
        VkImageCreateFlags ImageFlags = VK_NO_FLAGS;
        
        VkImageCreateInfo ImageCreateInfo = {};
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageCreateInfo.extent = VkExtent3D(Extent.X, Extent.Y, 1);
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.flags = ImageFlags;
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.mipLevels = 1;
        ImageCreateInfo.arrayLayers = 1;
        ImageCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        FRHIImageHandle Handle = ImagePool.Allocate();
        FVulkanImage* VulkanImage =ImagePool.GetResource(Handle);

        VmaAllocationCreateFlags AllocationFlags = VK_NO_FLAGS;
        
        VulkanImage->Allocation = MemoryAllocator->AllocateImage(&ImageCreateInfo, AllocationFlags, &VulkanImage->Image, "");

        // Create Image View
        VkImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = VulkanImage->Image;
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;

        VK_CHECK(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &VulkanImage->ImageView));

        return Handle;
    }

    FRHIImageHandle FVulkanRenderContext::CreateDepthImage(const FImageSpecification& ImageSpec)
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
    
        // Pipeline stage flags for synchronization
        VkPipelineStageFlags srcStageMask = 0;
        VkPipelineStageFlags dstStageMask = 0;
    
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
    
                // Set the access masks based on image layouts
                imageMemBarrier.srcAccessMask = GetImageAccessMask(Barrier->FromLayout);
                imageMemBarrier.dstAccessMask = (Barrier->ToLayout == EImageLayout::Default) ? GetImageAccessMask(FromVkImageLayout(VulkanImage->DefaultLayout)) : GetImageAccessMask(Barrier->ToLayout);
    
                imageMemBarrier.oldLayout = ToVkImageLayout(Barrier->FromLayout);
                imageMemBarrier.newLayout = (Barrier->ToLayout == EImageLayout::Default) ? VulkanImage->DefaultLayout : ToVkImageLayout(Barrier->ToLayout);

                VulkanImage->CurrentLayout = imageMemBarrier.newLayout;
                    
                imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemBarrier.image = VulkanImage->Image;
                imageMemBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    
                // Set pipeline stage masks based on the operation
                srcStageMask |= GetPipelineStageForAccess(imageMemBarrier.srcAccessMask);
                dstStageMask |= GetPipelineStageForAccess(imageMemBarrier.dstAccessMask);
    
                break;
            }
    
            case FGPUBarrier::EType::Memory:
            {
                // Memory barriers would be handled here (if applicable).
                break;
            }
            }
        }
    
        // Execute the pipeline barrier
        if (numImageBarriers > 0)
        {
            vkCmdPipelineBarrier(
                CommandBuffer,
                srcStageMask,  // Source stage mask
                dstStageMask,  // Destination stage mask
                0,
                0, nullptr,
                0, nullptr,
                numImageBarriers, imageBarriers
            );
        }
    }

    FVulkanCommandList* FVulkanRenderContext::GetPrimaryCommandList() const
    {
        return PrimaryCommandList[CurrentFrameIndex];
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

        FRHIBufferHandle StagingBufferHandle = BufferPool.Allocate();
        FVulkanBuffer* VulkanStagingBuffer = BufferPool.GetResource(StagingBufferHandle);
        
        void* Memory = MemoryAllocator->MapMemory(Allocation);
        FMemory::MemCopy((char*)Memory + Offset, Data, Size);
        MemoryAllocator->UnmapMemory(Allocation);

        VulkanStagingBuffer->Allocation = Allocation;
        VulkanStagingBuffer->Buffer = VkStagingBuffer;
        VulkanStagingBuffer->Size = Size;

        
        CopyBuffer(Buffer, StagingBufferHandle);
        
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
    
    FCommandList* FVulkanRenderContext::BeginCommandList(ECommandBufferLevel Level, ECommandQueue CommandType, ECommandBufferUsage Usage)
    {

        bool bTransient = (Usage == ECommandBufferUsage::Transient);
        
        VkCommandBufferAllocateInfo AllocInfo = {};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = bTransient ? CommandPools.TransientCommandPools[0] : CommandPools.CommandPools[0];
        AllocInfo.level = (Level == ECommandBufferLevel::Primary) ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        AllocInfo.commandBufferCount = 1;

        
        FVulkanCommandList* VulkanCommandList = FMemory::New<FVulkanCommandList>();
        VulkanCommandList->CommandQueue = CommandType;
        VulkanCommandList->Type = Usage;

        /** Primary command buffer is managed seperately */
        if (Level == ECommandBufferLevel::Secondary)
        {
            CommandQueue.push(VulkanCommandList);
        }
        
        VK_CHECK(vkAllocateCommandBuffers(Device, &AllocInfo, &VulkanCommandList->CommandBuffer));
        
        VulkanCommandList->Fence = FencePool.Aquire();
        
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = bTransient ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_NO_FLAGS;
        BeginInfo.pInheritanceInfo = nullptr;
        
        VK_CHECK(vkBeginCommandBuffer(VulkanCommandList->CommandBuffer, &BeginInfo));

        return VulkanCommandList;
    }

    void FVulkanRenderContext::EndCommandList(FCommandList* CommandList)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;

        VK_CHECK(vkEndCommandBuffer(VulkanCommandList->CommandBuffer));
        
        
        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &VulkanCommandList->CommandBuffer;
        SubmitInfo.pWaitSemaphores = VulkanCommandList->WaitSemaphores.data();
        SubmitInfo.waitSemaphoreCount = VulkanCommandList->WaitSemaphores.size();
        SubmitInfo.pSignalSemaphores = VulkanCommandList->SignalSemaphores.data();
        SubmitInfo.signalSemaphoreCount = VulkanCommandList->SignalSemaphores.size();

        if (!VulkanCommandList->WaitSemaphores.empty())
        {
            VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            SubmitInfo.pWaitDstStageMask = WaitStages;
        }

        
        VK_CHECK(vkQueueSubmit(CommandQueues.GraphicsQueue, 1, &SubmitInfo, VulkanCommandList->Fence));
        VK_CHECK(vkWaitForFences(Device, 1, &VulkanCommandList->Fence, VK_TRUE, UINT64_MAX));

        VulkanCommandList->SignalSemaphores.clear();
        VulkanCommandList->WaitSemaphores.clear();
        
        VkCommandPool Pool = (CommandList->Type == ECommandBufferUsage::Transient) ? CommandPools.TransientCommandPools[0] : CommandPools.CommandPools[0];
        vkFreeCommandBuffers(Device, Pool, 1, &VulkanCommandList->CommandBuffer);
        
        FencePool.Release(VulkanCommandList->Fence);
        
    }
    
    void FVulkanRenderContext::BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo)
    {
        TVector<VkRenderingAttachmentInfo> ColorAttachments;
        VkRenderingAttachmentInfo DepthAttachment = {};

        for (int i = 0; i < PassInfo.ColorAttachments.size(); ++i)
        {
            const FRHIImageHandle& ImageHandle = PassInfo.ColorAttachments[i];

            FVulkanImage* Image = ImagePool.GetResource(ImageHandle);
            Assert(Image);
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = Image->ImageView;
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.ColorLoadOps[i] == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.ColorStoreOps[i] == ERenderLoadOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE; 
    
            Attachment.clearValue.color.float32[0] = PassInfo.ClearColorValues[i].R;
            Attachment.clearValue.color.float32[1] = PassInfo.ClearColorValues[i].G;
            Attachment.clearValue.color.float32[2] = PassInfo.ClearColorValues[i].B;
            Attachment.clearValue.color.float32[3] = PassInfo.ClearColorValues[i].A;
    
            ColorAttachments.push_back(Attachment);
        }
        
        const FRHIImageHandle& ImageHandle = PassInfo.DepthAttachment;
        if (ImageHandle != FRenderHandle::InvalidHandle())
        {
            FVulkanImage* Image = ImagePool.GetResource(ImageHandle);
            Assert(Image);
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = Image->ImageView;
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

    void FVulkanRenderContext::EndRenderPass(FCommandList* CommandList)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;

        vkCmdEndRendering(VulkanCommandList->CommandBuffer);
        
    }

    void FVulkanRenderContext::ClearColor(FCommandList* CommandList, const FColor& Color)
    {
        FVulkanCommandList* VulkanCommandList = (FVulkanCommandList*)CommandList;

        FRHIImageHandle ImageHandle = Swapchain->GetCurrentImage();
        FVulkanImage* VulkanImage = ImagePool.GetResourceChecked(ImageHandle);

        VkClearColorValue ClearColorValue;
        ClearColorValue.float32[0] = Color.R; // Red
        ClearColorValue.float32[1] = Color.G; // Green
        ClearColorValue.float32[2] = Color.B; // Blue
        ClearColorValue.float32[3] = Color.A; // Alpha

        VkImageSubresourceRange SubresourceRange = {};
        SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        SubresourceRange.baseMipLevel = 0;
        SubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        SubresourceRange.baseArrayLayer = 0;
        SubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        /** Transfer the image from presentation (swapchain image) to a layout suitable for transfer operations */
        {
            FGPUBarrier Barriers[] =
            {
                FGPUBarrier::Image(ImageHandle, EImageLayout::PresentSource, EImageLayout::TransferDestination)
            };

            Barrier(Barriers, std::size(Barriers), CommandList);
        }
        
        vkCmdClearColorImage(VulkanCommandList->CommandBuffer, VulkanImage->Image, VulkanImage->CurrentLayout, &ClearColorValue, 1, &SubresourceRange);


        /** Transfer back to it's desired default layout */
        {
            FGPUBarrier Barriers[] =
            {
                FGPUBarrier::Image(ImageHandle, EImageLayout::TransferDestination)
            };

            Barrier(Barriers, std::size(Barriers), CommandList);
        }
        
    }


    void FVulkanRenderContext::SetVulkanObjectName(FString Name, VkObjectType ObjectType, uint64 Handle)
    {
        VkDebugUtilsObjectNameInfoEXT NameInfo = {};
        NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        NameInfo.objectType = ObjectType;
        NameInfo.objectHandle = Handle;
        NameInfo.pObjectName = Name.c_str();

        DebugUtils.DebugUtilsObjectNameEXT(Device, &NameInfo);
    }
    
}


#endif