#include "VulkanRenderContext.h"
#include "Renderer/Swapchain.h"
#include "Renderer/Pipeline.h"
#include "VulkanMacros.h"
#include "vk-bootstrap/src/VkBootstrap.h"
#include "VulkanMemoryAllocator.h"
#include "Renderer/CommandBuffer.h"
#include "VulkanSwapchain.h"
#include "Core/Windows/Window.h"
#include "Log/Log.h"
#include "Renderer/Image.h"


namespace Lumina
{
    VkBool32 VKAPI_PTR VkDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
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
                LOG_ERROR("Vulkan Validation Layer: {0}", pCallbackData->pMessage);
                break;
        }

        // Return VK_FALSE to indicate the application should not abort
        return VK_FALSE;
    }
    
    FVulkanRenderContext::~FVulkanRenderContext()
    {
        LOG_TRACE("VulkanRenderContext: Shutting Down");
        
        Swapchain->DestroySwapchain();
        Swapchain->DestroySurface();
        
        vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
        
        vkDestroyCommandPool(Device, CommandPool, nullptr);

        FVulkanMemoryAllocator::Get()->ClearAllAllocations();
        
        vkDestroyDevice(Device, nullptr);

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        func(VulkanInstance, VulkanRenderContextFunctions.DebugMessenger, nullptr);
        
        vkDestroyInstance(VulkanInstance, nullptr);
    }

    void FVulkanRenderContext::Initialize()
    {
        AssertMsg(glfwVulkanSupported(), "Vulkan Is Not Supported!");
        
        LOG_TRACE("Vulkan Render Context: Initializing");
        
        vkb::InstanceBuilder Builder;
        auto InstBuilder = Builder.set_app_name("Lumina Engine")
        .request_validation_layers()
        .use_default_debug_messenger()
        .set_debug_callback(VkDebugCallback)
        .enable_extension("VK_EXT_debug_utils")
        .require_api_version(1, 3, 0)
        .build();

        VulkanRenderContextFunctions.DebugMessenger = InstBuilder->debug_messenger;
        VulkanRenderContextFunctions.DebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(
        vkGetInstanceProcAddr(InstBuilder.value(), "vkSetDebugUtilsObjectNameEXT"));

        
        VulkanInstance = InstBuilder.value();
        
        VkPhysicalDeviceVulkan13Features features = {};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features.dynamicRendering = true;
        features.synchronization2 = true;

        VkPhysicalDeviceVulkan12Features features12 = {};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing =  true;
        features12.descriptorBindingPartiallyBound = true;
        
        VkPhysicalDeviceFeatures device_features = {};
        device_features.samplerAnisotropy = VK_TRUE;
        device_features.sampleRateShading = VK_TRUE;
        
        vkb::PhysicalDeviceSelector selector{ InstBuilder.value() };
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .require_separate_compute_queue()
            .defer_surface_initialization()
            .select()
            .value();

        /** Required for ImGui viewports. */
        physicalDevice.enable_extension_if_present("VK_KHR_dynamic_rendering");
        physicalDevice.enable_extension_if_present("VK_EXT_conservative_rasterization");
        
        vkb::DeviceBuilder deviceBuilder{ physicalDevice };
        vkb::Device vkbDevice = deviceBuilder.build().value();

        Device = vkbDevice.device;
        PhysicalDevice = physicalDevice.physical_device;

        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicslDeviceMemoryProperties);
        vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

        LOG_INFO("Creating Vulkan Device: {0}", PhysicalDeviceProperties.deviceName);
        
        GeneralQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        QueueFamilyIndex.Graphics =     vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        QueueFamilyIndex.Compute =      vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        QueueFamilyIndex.Transfer =     vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        QueueFamilyIndex.Present =      QueueFamilyIndex.Graphics;
        
        VkCommandPoolCreateInfo CmdPoolCreateInfo = {};
        CmdPoolCreateInfo.sType =               VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CmdPoolCreateInfo.queueFamilyIndex =    GetQueueFamilyIndex().Graphics;
        CmdPoolCreateInfo.flags =               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        vkCreateCommandPool(Device, &CmdPoolCreateInfo, nullptr, &CommandPool);

        FSwapchainSpec SwapchainSpec;
        SwapchainSpec.Window =          Config.Window;
        SwapchainSpec.Extent.x =        (int)Config.Window->GetWidth();
        SwapchainSpec.Extent.y =        (int)Config.Window->GetHeight();
        SwapchainSpec.FramesInFlight =  Config.FramesInFlight;
        
        Swapchain = MakeRefPtr<FVulkanSwapchain>(SwapchainSpec);
        Swapchain->CreateSurface(this, SwapchainSpec);
        Swapchain->CreateSwapchain(this, SwapchainSpec);
        physicalDevice.surface = GetSwapchain<FVulkanSwapchain>()->GetSurface();
        PhysicalDevice = physicalDevice;

        CommandBuffers.resize(Config.FramesInFlight);

        for (int i = 0; i < CommandBuffers.size(); ++i)
        {
            CommandBuffers[i] = FCommandBuffer::Create(ECommandBufferLevel::PRIMARY, ECommandBufferType::GENERAL, ECommandType::GENERAL);
            CommandBuffers[i]->SetFriendlyName("Command Buffer : " + eastl::to_string(i));
        }

        CurrentCommandBuffer = CommandBuffers[0];
        
        uint32 Count = 100 * Config.FramesInFlight;

        TInlineVector<VkDescriptorPoolSize, 8> PoolSizes =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER,							Count }, 
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,			Count },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,					Count },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,					Count },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,					Count },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,			Count },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,					Count },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,			Count },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,				Count }
        };

        VkDescriptorPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.maxSets = Count;
        PoolInfo.poolSizeCount = (uint32)PoolSizes.size();
        PoolInfo.pPoolSizes = PoolSizes.data();
        PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &DescriptorPool);
    }


    TVector<VkDescriptorSet> FVulkanRenderContext::AllocateDescriptorSets(VkDescriptorSetLayout InLayout, uint32 InCount)
    {
        TVector<VkDescriptorSet> sets(InCount);

        VkDescriptorSetAllocateInfo AllocateInfo = {};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        AllocateInfo.descriptorPool = DescriptorPool;
        AllocateInfo.descriptorSetCount = InCount;
        AllocateInfo.pSetLayouts = &InLayout;

        VK_CHECK(vkAllocateDescriptorSets(Device, &AllocateInfo, sets.data()));

        return sets;
    }

    void FVulkanRenderContext::FreeDescriptorSets(const TVector<VkDescriptorSet>& InSets)
    {
        vkFreeDescriptorSets(Device, DescriptorPool, (uint32)InSets.size(), InSets.data());
    }

    VkCommandBuffer FVulkanRenderContext::AllocateTransientCommandBuffer()
    {
        VkCommandBufferAllocateInfo AllocateInfo = {};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocateInfo.commandPool = CommandPool;
        AllocateInfo.commandBufferCount = 1;
        AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkCommandBuffer CmdBuffer;
        vkAllocateCommandBuffers(Device, &AllocateInfo, &CmdBuffer);

        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(CmdBuffer, &BeginInfo);

        return CmdBuffer;
    }

    void FVulkanRenderContext::ExecuteTransientCommandBuffer(VkCommandBuffer CmdBuffer)
    {
        vkEndCommandBuffer(CmdBuffer);

        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.pCommandBuffers = &CmdBuffer;
        SubmitInfo.commandBufferCount = 1;

        VkFence Fence;
        VkFenceCreateInfo FenceCreateInfo = {};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence);

        vkQueueSubmit(GeneralQueue, 1, &SubmitInfo, Fence);
        vkWaitForFences(Device, 1, &Fence, VK_TRUE, UINT64_MAX);

        vkResetCommandPool(Device, CommandPool, 0);
        vkFreeCommandBuffers(Device, CommandPool, 1, &CmdBuffer);

        vkDestroyFence(Device, Fence, nullptr);
    }
}
