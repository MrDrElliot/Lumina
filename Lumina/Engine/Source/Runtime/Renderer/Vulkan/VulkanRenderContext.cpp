#include "VulkanRenderContext.h"

#include "VkBootstrap.h"
#include "VulkanHelpers.h"

namespace Lumina
{
    FVulkanRenderContext::FVulkanRenderContext()
    {
        CreateVkInstance();
    }

    FVulkanRenderContext::~FVulkanRenderContext()
    {
    }

    void FVulkanRenderContext::Draw(float DeltaTime)
    {
        /* Don't want to render if we haven't initialized yet */
        if(!bInitialized || !ActiveSwapChain) return;
        

        /* Wait until the GPU has finished rendering the previous frame */
        VK_CHECK(vkWaitForFences(Device, 1, &GetCurrentFrame().RenderFence, true, 1000000000));
        VK_CHECK(vkResetFences(Device, 1, &GetCurrentFrame().RenderFence));

        
        /* Request the rendered image from the swap chain */
        uint32_t SwapChainImageIndex = 0;
        VK_CHECK(vkAcquireNextImageKHR(Device, ActiveSwapChain, 1000000000, GetCurrentFrame().SwapchainSemaphore, nullptr, &SwapChainImageIndex));


        
    }

    void FVulkanRenderContext::InternalInit()
    {
        /* Initialize Command Buffers */
        InitCommands();

        /* Initialize Fences and Semaphores */
        InitSyncStructures();

        
        bInitialized = true;
    }

    void FVulkanRenderContext::InitCommands()
    {
        VkCommandPoolCreateInfo CommandPoolInfo = Vulkan::CommandPoolCreateInfo(GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        /* Initialize command buffers and pools for each frame */
        for(auto i = 0; i < FRAME_OVERLAP; i++)
        {
            /* Create Command Pool */
            VK_CHECK(vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &Frames[i].CommandPool));

            VkCommandBufferAllocateInfo AllocInfo = Vulkan::CommandBufferAllocateInfo(Frames[i].CommandPool);

            /* Allocate Command Buffers */
            VK_CHECK(vkAllocateCommandBuffers(Device, &AllocInfo, &Frames[i].CommandBuffer));
        }
    }

    void FVulkanRenderContext::InitSyncStructures()
    {
        VkFenceCreateInfo FenceInfo = Vulkan::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo SemaphoreInfo = Vulkan::SemaphoreCreateInfo();

        for(auto i = 0; i <FRAME_OVERLAP; i++)
        {
            /* Create Fence */
            VK_CHECK(vkCreateFence(Device, &FenceInfo, nullptr, &Frames[i].RenderFence));

            /* Create Swap Chain Semaphore */
            VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &Frames[i].SwapchainSemaphore));

            /* Create Render Semaphore */
            VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &Frames[i].RenderSemaphore));
            
        }
    }

    void FVulkanRenderContext::InitializedDependencies(vkb::Device InDevice, vkb::PhysicalDevice InPhysicalDevice)
    {
        Device = InDevice;
        PhysicalDevice = InPhysicalDevice;

        GraphicsQueue = Device.get_queue(vkb::QueueType::graphics).value();
        GraphicsQueueFamily = Device.get_queue_index(vkb::QueueType::graphics).value();
        
        InternalInit();
    }

    void FVulkanRenderContext::ClearSwapChain()
    {
        ActiveSwapChain = nullptr;
    }

    void FVulkanRenderContext::CreateVkInstance()
    {
        vkb::InstanceBuilder Builder;

        auto Inst = Builder
        .set_app_name("Lumina")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

        VkbInstance = Inst.value();

    }
}
