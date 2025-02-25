
#include "Core/Math/Alignment.h"
#include "Renderer/Buffer.h"
#include "Renderer/RHI/Vulkan/VulkanCommon.h"
#include "Renderer/RHI/Vulkan/VulkanMemoryAllocator.h"
#ifdef LUMINA_RENDERER_VULKAN
#include "VulkanRenderDevice.h"
#include "src/VkBootstrap.h"
#include <vulkan/vulkan.hpp>

namespace Lumina
{

    VkBufferUsageFlags convert(TBitFlags<EDeviceBufferUsage> Usage) 
    {
        VkBufferUsageFlags result = 0;

        if (Usage.IsFlagSet(EDeviceBufferUsage::VERTEX_BUFFER))
        {
            result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
    
        if (Usage.IsFlagSet(EDeviceBufferUsage::INDEX_BUFFER))
        {
            result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
    
        if (Usage.IsFlagSet(EDeviceBufferUsage::UNIFORM_BUFFER))
        {
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
    
        if (Usage.IsFlagSet(EDeviceBufferUsage::UNIFORM_BUFFER_DYNAMIC))
        {
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        if (Usage.IsFlagSet(EDeviceBufferUsage::STORAGE_BUFFER))
        {
            result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (Usage.IsFlagSet(EDeviceBufferUsage::STORAGE_BUFFER_DYNAMIC))
        {
            result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (Usage.IsFlagSet(EDeviceBufferUsage::STAGING_BUFFER))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

        if (Usage.IsFlagSet(EDeviceBufferUsage::SHADER_DEVICE_ADDRESS))
        {
            result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        }

        return result;
    }

    
    FVulkanRenderDevice::FVulkanRenderDevice()
    {
        LogicalDevice = VK_NULL_HANDLE;
        PhysicalDevice = VK_NULL_HANDLE;
    }

    void FVulkanRenderDevice::Initialize()
    {
        MemoryAllocator = FMemory::New<FVulkanMemoryAllocator>();
    }

    void FVulkanRenderDevice::CreateDevice(vkb::Instance Instance)
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
        CommandQueues.PresentQueue = vkbDevice.get_queue(vkb::QueueType::present).value();
        CommandQueues.TransferQueue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
        CommandQueues.ComputeQueue = vkbDevice.get_queue(vkb::QueueType::compute).value();
        
        CommandQueues.GraphicsQueueIndex =     vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        CommandQueues.ComputeQueueIndex =      vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        CommandQueues.TransferQueueIndex =     vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        CommandQueues.PresentQueueIndex =      vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        
        LogicalDevice = vkbDevice.device;
        PhysicalDevice = physicalDevice.physical_device;
        
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
        vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
    }

    void FVulkanRenderDevice::SetPhysicalDevice(VkPhysicalDevice Device)
    {
        PhysicalDevice = Device;
    }

    void FVulkanRenderDevice::SetLogicalDevice(VkDevice Device)
    {
        LogicalDevice = Device;
    }

    uint64 FVulkanRenderDevice::GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EDeviceBufferUsage> Usage)
    {
        uint64 MinAlignment = 1;  // Default alignment

        if(Usage.AreAnyFlagsSet(EDeviceBufferUsage::UNIFORM_BUFFER, EDeviceBufferUsage::UNIFORM_BUFFER_DYNAMIC))
        {
            MinAlignment = PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
        }
        if(Usage.AreAnyFlagsSet(EDeviceBufferUsage::STORAGE_BUFFER, EDeviceBufferUsage::STORAGE_BUFFER_DYNAMIC))
        {
            MinAlignment = PhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
        }

        return Math::GetAligned(Size, MinAlignment);
    }

    FRHIBufferHandle FVulkanRenderDevice::CreateBuffer(TBitFlags<EDeviceBufferUsage> UsageFlags, TBitFlags<EDeviceBufferMemoryUsage> MemoryUsage, uint32 Size)
    {
        VmaAllocationCreateFlags VmaFlags = 0;
        
        VkBufferCreateInfo BufferCreateInfo = {};
        ZeroVkStruct(BufferCreateInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
        BufferCreateInfo.size = GetAlignedSizeForBuffer(Size, UsageFlags);
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = convert(UsageFlags);
        BufferCreateInfo.flags = 0;

        VkBuffer Buffer;

        MemoryAllocator->AllocateBuffer(&BufferCreateInfo, VmaFlags, &Buffer, "");
        
    }

    void FVulkanRenderDevice::UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset)
    {
        
    }
}


#endif