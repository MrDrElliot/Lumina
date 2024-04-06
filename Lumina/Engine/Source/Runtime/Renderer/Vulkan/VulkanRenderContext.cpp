#include "VulkanRenderContext.h"

#include "VkBootstrap.h"
#include "VulkanHelpers.h"
#include "VulkanSwapChain.h"

namespace Lumina
{
    FVulkanRenderContext::FVulkanRenderContext()
    {
        CreateVkInstance();
    }

    FVulkanRenderContext::~FVulkanRenderContext()
    {
        for (auto Frame : Frames)
        {
            vkDestroySemaphore(Device, Frame.RenderSemaphore, nullptr);
            vkDestroySemaphore(Device, Frame.SwapchainSemaphore, nullptr);
            vkDestroyFence(Device, Frame.RenderFence, nullptr);
            vkDestroyCommandPool(Device, Frame.CommandPool, nullptr);
        }

        vkDestroyDevice(Device, nullptr);

    }

    void FVulkanRenderContext::Draw(float DeltaTime)
    {
        /* Don't want to render if we haven't initialized yet */
        if(!bInitialized || !ActiveSwapChain) return;
        

        /* Wait until the GPU has finished rendering the previous frame */
        VK_CHECK(vkWaitForFences(Device, 1, &GetCurrentFrame().RenderFence, true, 1000000000));
        VK_CHECK(vkResetFences(Device, 1, &GetCurrentFrame().RenderFence));

        GetCurrentFrame().DeletionQueue.Flush();
        
        /* Request the rendered image from the swap chain */
        uint32_t SwapChainImageIndex = 0;
        VK_CHECK(vkAcquireNextImageKHR(Device, ActiveSwapChain->GetSwapChain(), 1000000000, GetCurrentFrame().SwapchainSemaphore, nullptr, &SwapChainImageIndex));

        /* Get the current frame's command buffer */
        VkCommandBuffer Cmd = GetCurrentFrame().CommandBuffer;

        
        /* Reset the commad buffer since we know it's finished, we need to do this to start recording again */
        VK_CHECK(vkResetCommandBuffer(Cmd, 0));
        

        /* Begin recording to the command buffer of this frame */
        VkCommandBufferBeginInfo CmdBeginInfo = Vulkan::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        VK_CHECK(vkBeginCommandBuffer(Cmd, &CmdBeginInfo));

        // Transition our main draw image into general layout so we can write into it
        // We will overwrite it all so we dont care about what was the older layout
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetDrawImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        DrawBackground(Cmd);

        // Transition the draw image and the swapchain image into their correct transfer layouts
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetDrawImage().Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Execute a copy from the draw image into the swapchain */
        Vulkan::CopyImageToImage(Cmd, ActiveSwapChain->GetDrawImage().Image, ActiveSwapChain->GetImages()[SwapChainImageIndex], ActiveSwapChain->GetDrawExtent2D(), ActiveSwapChain->GetExtent2D());

        // Set swapchain image layout to Present so we can show it on the screen
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        /* End the command buffer, commands cannot be added to it after this point */
        VK_CHECK(vkEndCommandBuffer(Cmd));

        VkCommandBufferSubmitInfo CmdInfo = Vulkan::CommandBufferSubmitInfo(Cmd);
        
        VkSemaphoreSubmitInfo WaitInfo = Vulkan::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().SwapchainSemaphore);
        VkSemaphoreSubmitInfo SignalInfo = Vulkan::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().RenderSemaphore);

        VkSubmitInfo2 Submit = Vulkan::SubmitInfo(&CmdInfo, &SignalInfo, &WaitInfo);

        VK_CHECK(vkQueueSubmit2(GraphicsQueue, 1, &Submit, GetCurrentFrame().RenderFence));

        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.pNext = nullptr;
        PresentInfo.pSwapchains = &ActiveSwapChain->GetSwapChain();
        PresentInfo.swapchainCount = 1;
        PresentInfo.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pImageIndices = &SwapChainImageIndex;

        VK_CHECK(vkQueuePresentKHR(GraphicsQueue, &PresentInfo));
        
        
        FrameNumber++;
    }

    void FVulkanRenderContext::DrawBackground(VkCommandBuffer InBuffer)
    {
        VkClearColorValue clearValue;
        float flash = abs(sin(FrameNumber / 120.f));
        clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

        VkImageSubresourceRange clearRange = Vulkan::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        // bind the gradient drawing compute pipeline
        vkCmdBindPipeline(InBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, GradientPipeline);

        // bind the descriptor set containing the draw image for the compute pipeline
        vkCmdBindDescriptorSets(InBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, GradientPipelineLayout, 0, 1, &DrawImageDescriptors, 0, nullptr);

        // execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
        vkCmdDispatch(InBuffer, std::ceil(ActiveSwapChain->GetDrawExtent().width / 16.0), std::ceil(ActiveSwapChain->GetDrawExtent().height / 16.0), 1);
    }

    void FVulkanRenderContext::FinalInitialization()
    {
        InternalInit();
    }

    void FVulkanRenderContext::InternalInit()
    {

        /* It's safe to execute anything that requires devices after this point */
        
        /* Initialize Command Buffers */
        InitCommands();

        /* Initialize Fences and Semaphores */
        InitSyncStructures();

        /* Initialize Descriptors */
        InitializeDescriptors();

        /* Initialize Pipelines */
        InitPipelines();
        
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

        for(auto i = 0; i < FRAME_OVERLAP; i++)
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
        
        /* Initialize Allocator */
        InitializeAllocator();
        
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

        Instance = Inst.value();

    }

    void FVulkanRenderContext::InitializeAllocator()
    {
        VmaAllocatorCreateInfo Info = {};
        Info.physicalDevice = PhysicalDevice;
        Info.device = Device;
        Info.instance = Instance;
        Info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        vmaCreateAllocator(&Info, &Allocator);

        
    }

    void FVulkanRenderContext::InitializeDescriptors()
    {
        std::vector<FDescriptorAllocator::FPoolSizeRatio> Sizes =
        {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
        };

        GlobalDescriptorAllocator.InitPool(Device, 10, Sizes);

        FDescriptorLayoutBuilder LayoutBuilder;
        LayoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        DrawImageDescriptorLayout = LayoutBuilder.Build(Device, VK_SHADER_STAGE_COMPUTE_BIT);

        DrawImageDescriptors = GlobalDescriptorAllocator.Allocate(Device, DrawImageDescriptorLayout);

        VkDescriptorImageInfo ImgInfo = {};
        ImgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        ImgInfo.imageView = ActiveSwapChain->GetDrawImage().ImageView;

        VkWriteDescriptorSet DrawImageWrite = {};
        DrawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        DrawImageWrite.pNext = nullptr;
        DrawImageWrite.dstBinding = 0;
        DrawImageWrite.dstSet = DrawImageDescriptors;
        DrawImageWrite.descriptorCount = 1;
        DrawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        DrawImageWrite.pImageInfo = &ImgInfo;

        vkUpdateDescriptorSets(Device, 1, &DrawImageWrite, 0, nullptr);
    }

    void FVulkanRenderContext::InitPipelines()
    {
        InitBackgroundPipelines();
    }

    void FVulkanRenderContext::InitBackgroundPipelines()
    {
        VkPipelineLayoutCreateInfo ComputeLayout{};
        ComputeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        ComputeLayout.pNext = nullptr;
        ComputeLayout.pSetLayouts = &DrawImageDescriptorLayout;
        ComputeLayout.setLayoutCount = 1;

        VK_CHECK(vkCreatePipelineLayout(Device, &ComputeLayout, nullptr, &GradientPipelineLayout));

        VkShaderModule* ComputeDrawShader;
        if (ComputeDrawShader = Vulkan::LoadShaderModule("Resources/Shaders/Gradient_Color.comp.spv", Device); ComputeDrawShader != nullptr)
        {
            LE_LOG_CRITICAL("Failed to create shader");
        }

        VkPipelineShaderStageCreateInfo stageinfo{};
        stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageinfo.pNext = nullptr;
        stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stageinfo.module = *ComputeDrawShader;
        stageinfo.pName = "main";

        VkComputePipelineCreateInfo ComputePipelineCreateInfo{};
        ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        ComputePipelineCreateInfo.pNext = nullptr;
        ComputePipelineCreateInfo.layout = GradientPipelineLayout;
        ComputePipelineCreateInfo.stage = stageinfo;
	
        VK_CHECK(vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1 ,&ComputePipelineCreateInfo, nullptr, &GradientPipeline));
    }
}
