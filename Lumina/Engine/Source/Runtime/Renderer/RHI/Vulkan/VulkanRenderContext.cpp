#include "VulkanRenderContext.h"

#include "VkBootstrap.h"
#include "VulkanMemoryAllocator.h"

#include "VulkanSwapchain.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Image.h"


namespace Lumina
{

    FVulkanRenderContext* FVulkanRenderContext::Instance = nullptr;
    
    FVulkanRenderContext::FVulkanRenderContext(const FRenderConfig& InConfig)
    {
        LE_LOG_INFO("Vulkan Render Context: Initializing");

        Instance = this;
        
        vkb::InstanceBuilder Builder;
        auto inst_ret = Builder.set_app_name("Lumina Engine")
        .request_validation_layers()
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

        DebugMessenger = inst_ret->debug_messenger;
        VulkanInstance = inst_ret.value();
        
        
        VkPhysicalDeviceVulkan13Features features{};
        features.dynamicRendering = true;
        features.synchronization2 = true;

        VkPhysicalDeviceVulkan12Features features12{};
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;
        
        vkb::PhysicalDeviceSelector selector{ inst_ret.value() };
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .require_separate_compute_queue()
            .require_separate_compute_queue()
            .defer_surface_initialization()
            .select()
            .value();


        vkb::DeviceBuilder deviceBuilder{ physicalDevice };
        vkb::Device vkbDevice = deviceBuilder.build().value();

        Device = vkbDevice.device;
        PhysicalDevice = physicalDevice.physical_device;
        

        GeneralQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        QueueFamilyIndex.Graphics = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        QueueFamilyIndex.Compute = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        QueueFamilyIndex.Transfer = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        QueueFamilyIndex.Present = QueueFamilyIndex.Graphics;
        
        VkCommandPoolCreateInfo CmdPoolCreateInfo = {};
        CmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CmdPoolCreateInfo.queueFamilyIndex = GetQueueFamilyIndex().Graphics;
        CmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        vkCreateCommandPool(Device, &CmdPoolCreateInfo, nullptr, &CommandPool);

        FSwapchainSpec SwapchainSpec;
        SwapchainSpec.Window = InConfig.Window;
        SwapchainSpec.Extent.x = InConfig.Window->GetWidth();
        SwapchainSpec.Extent.y = InConfig.Window->GetHeight();
        SwapchainSpec.FramesInFlight = 2;
        
        Swapchain = std::make_shared<FVulkanSwapchain>(SwapchainSpec);
        Swapchain->CreateSurface(SwapchainSpec);
        Swapchain->CreateSwapchain(SwapchainSpec);
        

        physicalDevice.surface = Swapchain->GetSurface();
        PhysicalDevice = physicalDevice;


        FVulkanMemoryAllocator::Init();

        FImageSamplerSpecification Nearest = {};
        Nearest.MinFilteringMode = ESamplerFilteringMode::LINEAR;
        Nearest.MagFilteringMode = ESamplerFilteringMode::NEAREST;
        Nearest.MipMapFilteringMode = ESamplerFilteringMode::LINEAR;
        Nearest.AddressMode = ESamplerAddressMode::REPEAT;
        Nearest.MinLOD = 0.0f;
        Nearest.MaxLOD = 1000.0f;
        Nearest.LODBias = 0.0f;
        Nearest.AnisotropicFilteringLevel = 1;

        NearestSampler = FImageSampler::Create(Nearest);
        
        // Initializing linear filtration sampler
        FImageSamplerSpecification Linear = {};
        Linear.MinFilteringMode = ESamplerFilteringMode::LINEAR;
        Linear.MagFilteringMode = ESamplerFilteringMode::LINEAR;
        Linear.MipMapFilteringMode = ESamplerFilteringMode::LINEAR;
        Linear.AddressMode = ESamplerAddressMode::REPEAT;
        Linear.MinLOD = 0.0f;
        Linear.MaxLOD = 1000.0f;
        Linear.LODBias = 0.0f;
        Linear.AnisotropicFilteringLevel = 1;

        LinearSampler = FImageSampler::Create(Linear);
        
    }

    FVulkanRenderContext::~FVulkanRenderContext()
    {
        FVulkanRenderContext::Destroy();
    }

    void FVulkanRenderContext::Destroy()
    {
        vkDeviceWaitIdle(Device);
        
        FVulkanMemoryAllocator::Destroy();
        
        Swapchain->DestroySwapchain();
        Swapchain->DestroySurface();

        vkDestroyCommandPool(Device, CommandPool, nullptr);
        vkDestroyDevice(Device, nullptr);

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        func(VulkanInstance, DebugMessenger, nullptr);
        
        vkDestroyInstance(VulkanInstance, nullptr);


        LE_LOG_WARN("Vulkan Render Context: Shutting Down");
    }

    VkCommandBuffer FVulkanRenderContext::AllocateTransientCommandBuffer()
    {
        VkCommandBufferAllocateInfo AllocateInfo = {};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocateInfo.commandPool = Instance->CommandPool;
        AllocateInfo.commandBufferCount = 1;
        AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkCommandBuffer CmdBuffer;
        vkAllocateCommandBuffers(Instance->Device, &AllocateInfo, &CmdBuffer);

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

        vkCreateFence(Instance->Device, &FenceCreateInfo, nullptr, &Fence);

        vkQueueSubmit(Instance->GeneralQueue, 1, &SubmitInfo, Fence);
        vkWaitForFences(Instance->Device, 1, &Fence, VK_TRUE, UINT64_MAX);

        vkResetCommandPool(Instance->Device, Instance->CommandPool, 0);
        vkFreeCommandBuffers(Instance->Device, Instance->CommandPool, 1, &CmdBuffer);

        vkDestroyFence(Instance->Device, Fence, nullptr);
    }
}
