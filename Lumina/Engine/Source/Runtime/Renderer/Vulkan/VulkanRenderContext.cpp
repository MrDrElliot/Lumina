#include "VulkanRenderContext.h"
#include <imgui/imgui.h>
#include <vk-bootstrap/src/VkBootstrap.h>
#include "VulkanHelpers.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"



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

    void FVulkanRenderContext::ImGuiDraw(float DeltaTime)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::Begin("Background Editor"))
        {
            FComputeEffect& Selected = BackgroundEffects[CurrentBackgroundEffect];
            ImGui::Text("Selected Effect", Selected.Name);
            ImGui::SliderInt("Effect Index", &CurrentBackgroundEffect, 0, BackgroundEffects.size() - 1);

            ImGui::InputFloat4("data1",(float*)& Selected.Data.data1);
            ImGui::InputFloat4("data2",(float*)& Selected.Data.data2);
            ImGui::InputFloat4("data3",(float*)& Selected.Data.data3);
            ImGui::InputFloat4("data4",(float*)& Selected.Data.data4);
            ImGui::End();   
        }
        
        ImGui::Render();
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

        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetDrawImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawGeometry(Cmd);

        // Transition the draw image and the swapchain image into their correct transfer layouts
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetDrawImage().Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Execute a copy from the draw image into the swapchain */
        Vulkan::CopyImageToImage(Cmd, ActiveSwapChain->GetDrawImage().Image, ActiveSwapChain->GetImages()[SwapChainImageIndex], ActiveSwapChain->GetDrawExtent2D(), ActiveSwapChain->GetExtent2D());

        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawImGui(Cmd, ActiveSwapChain->GetImageViews()[SwapChainImageIndex]);
        
        // Set swapchain image layout to Present so we can show it on the screen
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

    void FVulkanRenderContext::DrawGeometry(VkCommandBuffer InCmd)
    {
        VkRenderingAttachmentInfo colorAttachment = Vulkan::RenderingAttachmentInfo(ActiveSwapChain->GetDrawImage().ImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);

        VkRenderingInfo renderInfo = Vulkan::RenderingInfo(ActiveSwapChain->GetDrawExtent2D(), &colorAttachment, nullptr);
        vkCmdBeginRendering(InCmd, &renderInfo);

        vkCmdBindPipeline(InCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, TrianglePipeline);

        //set dynamic viewport and scissor
        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = ActiveSwapChain->GetDrawExtent2D().width;
        viewport.height = ActiveSwapChain->GetDrawExtent2D().height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        vkCmdSetViewport(InCmd, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = ActiveSwapChain->GetDrawExtent2D().width;
        scissor.extent.height = ActiveSwapChain->GetDrawExtent2D().height;

        vkCmdSetScissor(InCmd, 0, 1, &scissor);

        vkCmdDraw(InCmd, 3, 1, 0, 0);


        vkCmdBindPipeline(InCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, MeshPipeline);

        FGPUDrawPushConstants push_constants;
        push_constants.WorldMatrix = glm::mat4{ 1.f };
        push_constants.VertexBuffer = Rect.VertexBufferAddress;

        push_constants.VertexBuffer = testMeshes[2]->MeshBuffers.VertexBufferAddress;

        vkCmdPushConstants(InCmd, MeshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FGPUDrawPushConstants), &push_constants);
        vkCmdBindIndexBuffer(InCmd, Rect.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(InCmd, 6, 1, 0, 0, 0);

        vkCmdEndRendering(InCmd);
    }

    void FVulkanRenderContext::DrawBackground(VkCommandBuffer InBuffer)
    {

        FComputeEffect& Effect = BackgroundEffects[CurrentBackgroundEffect];
        

        VkImageSubresourceRange clearRange = Vulkan::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        // bind the gradient drawing compute pipeline
        vkCmdBindPipeline(InBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Effect.Pipeline);

        // bind the descriptor set containing the draw image for the compute pipeline
        vkCmdBindDescriptorSets(InBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, GradientPipelineLayout, 0, 1, &DrawImageDescriptors, 0, nullptr);

        FComputePushConstants pc;
        pc.data1 = glm::vec4(1, 0, 0, 1);
        pc.data2 = glm::vec4(0, 0, 1, 1);

        vkCmdPushConstants(InBuffer, GradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(FComputePushConstants), &Effect.Data);

        
        // execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
        vkCmdDispatch(InBuffer, std::ceil(ActiveSwapChain->GetDrawExtent().width / 16.0), std::ceil(ActiveSwapChain->GetDrawExtent().height / 16.0), 1);
    }

    void FVulkanRenderContext::DrawImGui(VkCommandBuffer InBuffer, VkImageView TargetViewImage)
    {
        VkRenderingAttachmentInfo colorAttachment = Vulkan::RenderingAttachmentInfo(TargetViewImage, nullptr, VK_IMAGE_LAYOUT_GENERAL);
        VkRenderingInfo renderInfo = Vulkan::RenderingInfo(ActiveSwapChain->GetExtent2D(), &colorAttachment, nullptr);

        vkCmdBeginRendering(InBuffer, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), InBuffer);

        vkCmdEndRendering(InBuffer);
    }

    FAllocatedBuffer FVulkanRenderContext::CreateBuffer(size_t Size, VkBufferUsageFlags InUsage, VmaMemoryUsage MemoryUsage)
    {
        VkBufferCreateInfo Info = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        Info.pNext = nullptr;
        Info.size = Size;
        Info.usage = InUsage;

        VmaAllocationCreateInfo VmaAllocInfo = {};
        VmaAllocInfo.usage = MemoryUsage;
        VmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        FAllocatedBuffer NewBuffer;

        VK_CHECK(vmaCreateBuffer(Allocator, &Info, &VmaAllocInfo, &NewBuffer.Buffer, &NewBuffer.Allocation, &NewBuffer.Info));

        return NewBuffer;
        
    }

    void FVulkanRenderContext::DestroyBuffer(const FAllocatedBuffer& Buffer)
    {
        vmaDestroyBuffer(Allocator, Buffer.Buffer, Buffer.Allocation);
    }

    FGPUMeshBuffers FVulkanRenderContext::UploadMesh(std::span<uint32_t> Indices, std::span<FVertex> Vertices)
    {
        const size_t vertexBufferSize = Vertices.size() * sizeof(FVertex);
        const size_t indexBufferSize = Indices.size() * sizeof(uint32_t);

        FGPUMeshBuffers newSurface;

        //create vertex buffer
        newSurface.VertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        //find the adress of the vertex buffer
        VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = newSurface.VertexBuffer.Buffer };
        newSurface.VertexBufferAddress = vkGetBufferDeviceAddress(Device, &deviceAdressInfo);

        //create index buffer
        newSurface.IndexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

        FAllocatedBuffer staging = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        VmaAllocationInfo Info;
        vmaGetAllocationInfo(Allocator, staging.Allocation, &Info);
        void* data = Info.pMappedData;

        // copy vertex buffer
        memcpy(data, Vertices.data(), vertexBufferSize);
        // copy index buffer
        memcpy((char*)data + vertexBufferSize, Indices.data(), indexBufferSize);

        ImmediateSubmit([&](VkCommandBuffer cmd) {
            VkBufferCopy vertexCopy{ 0 };
            vertexCopy.dstOffset = 0;
            vertexCopy.srcOffset = 0;
            vertexCopy.size = vertexBufferSize;

            vkCmdCopyBuffer(cmd, staging.Buffer, newSurface.VertexBuffer.Buffer, 1, &vertexCopy);

            VkBufferCopy indexCopy{ 0 };
            indexCopy.dstOffset = 0;
            indexCopy.srcOffset = vertexBufferSize;
            indexCopy.size = indexBufferSize;

            vkCmdCopyBuffer(cmd, staging.Buffer, newSurface.IndexBuffer.Buffer, 1, &indexCopy);
        });

        DestroyBuffer(staging);

        return newSurface;
    }

    void FVulkanRenderContext::ImmediateSubmit(std::function<void(VkCommandBuffer Buffer)>&& Function)
    {
        VK_CHECK(vkResetFences(Device, 1, &ImmediateFence));
        VK_CHECK(vkResetCommandBuffer(ImmediateCommandBuffer, 0));

        VkCommandBufferBeginInfo CmdBeginInfo = Vulkan::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(ImmediateCommandBuffer, &CmdBeginInfo));

        Function(ImmediateCommandBuffer);

        VK_CHECK(vkEndCommandBuffer(ImmediateCommandBuffer));

        VkCommandBufferSubmitInfo SubmitInfo = Vulkan::CommandBufferSubmitInfo(ImmediateCommandBuffer);
        VkSubmitInfo2 Submit = Vulkan::SubmitInfo(&SubmitInfo, nullptr, nullptr);

        VK_CHECK(vkQueueSubmit2(GraphicsQueue, 1, &Submit, ImmediateFence));
        
        VK_CHECK(vkWaitForFences(Device, 1, &ImmediateFence, true, 9999999999));

        
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

        /* Initialize ImGui */
        InitImGui();

        InitDefaultData();

        
        bInitialized = true;
    }

    void FVulkanRenderContext::InitCommands()
    {
        VkCommandPoolCreateInfo CommandPoolInfo = Vulkan::CommandPoolCreateInfo(GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        /* Initialize immediate command buffers */
        VK_CHECK(vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &ImmediateCommandPool));
        
        VkCommandBufferAllocateInfo ImmediateAllocInfo = Vulkan::CommandBufferAllocateInfo(ImmediateCommandPool, 1);
        VK_CHECK(vkAllocateCommandBuffers(Device, &ImmediateAllocInfo, &ImmediateCommandBuffer));
        

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

        VK_CHECK(vkCreateFence(Device, &FenceInfo, nullptr, &ImmediateFence));
        

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

    void FVulkanRenderContext::InitImGui()
    {
        VkDescriptorPoolSize PoolSizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

        VkDescriptorPoolCreateInfo PoolInfo =  {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        PoolInfo.maxSets = 1000;
        PoolInfo.poolSizeCount = (uint32_t)std::size(PoolSizes);
        PoolInfo.pPoolSizes = PoolSizes;


        VkDescriptorPool ImGuiPool;
        VK_CHECK(vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &ImGuiPool));

        ImGui::CreateContext();
        ImGuiIO& Io = ImGui::GetIO(); (void)Io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForVulkan(ActiveSwapChain->GetWindow()->GetWindow(), true);

        VkPipelineRenderingCreateInfo RenderPipeline = {};
        RenderPipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        RenderPipeline.pColorAttachmentFormats = &ActiveSwapChain->GetFormat();
        RenderPipeline.colorAttachmentCount = 1;

        ImGui_ImplVulkan_InitInfo InitInfo = {};
        InitInfo.PipelineRenderingCreateInfo = RenderPipeline;
        InitInfo.Instance = Instance;
        InitInfo.PhysicalDevice = PhysicalDevice;
        InitInfo.Device = Device;
        InitInfo.Queue = GraphicsQueue;
        InitInfo.DescriptorPool = ImGuiPool;
        InitInfo.MinImageCount = 3;
        InitInfo.ImageCount = 3;
        InitInfo.UseDynamicRendering = true;
        InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&InitInfo);

        ImmediateSubmit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

        ImGui_ImplVulkan_DestroyFontsTexture();

        
    }

    void FVulkanRenderContext::InitPipelines()
    {
        InitBackgroundPipelines();

        InitTrianglePipeline();
        InitMeshPipeline();
    }

    void FVulkanRenderContext::InitBackgroundPipelines()
    {
        VkPipelineLayoutCreateInfo ComputeLayout = {};
        ComputeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        ComputeLayout.pNext = nullptr;
        ComputeLayout.pSetLayouts = &DrawImageDescriptorLayout;
        ComputeLayout.setLayoutCount = 1;

        VkPushConstantRange PushConstant = {};
        PushConstant.offset = 0;
        PushConstant.size = sizeof(FComputePushConstants);
        PushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        ComputeLayout.pPushConstantRanges = &PushConstant;
        ComputeLayout.pushConstantRangeCount = 1;
        
        VK_CHECK(vkCreatePipelineLayout(Device, &ComputeLayout, nullptr, &GradientPipelineLayout));

        VkShaderModule GradientShader;
        if (GradientShader = Vulkan::LoadShaderModule("Resources/Shaders/Gradient_Color.comp.spv", Device); GradientShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create gradient shader");
        }


        VkShaderModule SkyShader;
        if(SkyShader = Vulkan::LoadShaderModule("Resources/Shaders/sky.comp.spv", Device); SkyShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create sky shader!");
        }

        

        VkPipelineShaderStageCreateInfo stageinfo{};
        stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageinfo.pNext = nullptr;
        stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stageinfo.module = GradientShader;
        stageinfo.pName = "main";

        VkComputePipelineCreateInfo ComputePipelineCreateInfo{};
        ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        ComputePipelineCreateInfo.pNext = nullptr;
        ComputePipelineCreateInfo.layout = GradientPipelineLayout;
        ComputePipelineCreateInfo.stage = stageinfo;

        FComputeEffect Gradient;
        Gradient.Layout = GradientPipelineLayout;
        Gradient.Name = "gradient";
        Gradient.Data = {};

        Gradient.Data.data1 = glm::vec4(1, 0, 0, 1);
        Gradient.Data.data2 = glm::vec4(0, 0, 1, 1);
	
        VK_CHECK(vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1 , &ComputePipelineCreateInfo, nullptr, &Gradient.Pipeline));
        
        ComputePipelineCreateInfo.stage.module = SkyShader;

        FComputeEffect Sky;
        Sky.Layout = GradientPipelineLayout;
        Sky.Name = "sky";
        Sky.Data = {};
        Sky.Data.data1 = glm::vec4(0.1, 0.2, 0.4 ,0.97);

        VK_CHECK(vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1, &ComputePipelineCreateInfo, nullptr, &Sky.Pipeline));

        BackgroundEffects.push_back(Gradient);
        BackgroundEffects.push_back(Sky);

        
    }

    void FVulkanRenderContext::InitTrianglePipeline()
    {
        VkShaderModule FragShader;
        if(FragShader = Vulkan::LoadShaderModule("Resources/Shaders/Colored_Triangle.frag.spv", Device); FragShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to created colored triangle frag shader!");
        }

        VkShaderModule VertShader;
        if(VertShader = Vulkan::LoadShaderModule("Resources/Shaders/Colored_Triangle.vert.spv", Device); VertShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create colored triangle vertex shader!");
        }

        VkPipelineLayoutCreateInfo LayoutInfo = Vulkan::PipelineLayoutCreateInfo();

        VK_CHECK(vkCreatePipelineLayout(Device, &LayoutInfo, nullptr, &TrianglePipelineLayout));

        FVulkanPipeline Pipeline;

        Pipeline.PipelineLayout = TrianglePipelineLayout;
        Pipeline.SetShaders(VertShader, FragShader);
        Pipeline.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        Pipeline.SetPolygonMode(VK_POLYGON_MODE_FILL);
        Pipeline.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        Pipeline.SetMultisampingNone();
        Pipeline.DisableBlending();
        Pipeline.DisableDepthTest();

        //connect the image format we will draw into, from draw image
        Pipeline.SetColorAttachmentFormat(ActiveSwapChain->GetDrawImage().ImageFormat);
        Pipeline.SetDepthFormat(VK_FORMAT_UNDEFINED);

        //finally build the pipeline
        TrianglePipeline = Pipeline.BuildPipeline(Device);
        

    }

    void FVulkanRenderContext::InitMeshPipeline()
    {
        VkShaderModule FragShader;
        if (FragShader = Vulkan::LoadShaderModule("Resources/Shaders/Colored_Triangle.frag.spv", Device); FragShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create colored triangle fragment shader!");
        }
        
        VkShaderModule VertexShader;
        if (VertexShader = Vulkan::LoadShaderModule("Resources/Shaders/Colored_Triangle_mesh.vert.spv", Device); VertexShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create triangle mesh vertex shader!");
        }

        FVulkanPipeline Pipeline;

        VkPushConstantRange bufferRange{};
        bufferRange.offset = 0;
        bufferRange.size = sizeof(FGPUDrawPushConstants);
        bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipeline_layout_info = Vulkan::PipelineLayoutCreateInfo();
        pipeline_layout_info.pPushConstantRanges = &bufferRange;
        pipeline_layout_info.pushConstantRangeCount = 1;

        VK_CHECK(vkCreatePipelineLayout(Device, &pipeline_layout_info, nullptr, &MeshPipelineLayout));

        
        Pipeline.PipelineLayout = MeshPipelineLayout;
        Pipeline.SetShaders(VertexShader, FragShader);
        Pipeline.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        Pipeline.SetPolygonMode(VK_POLYGON_MODE_FILL);
        Pipeline.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        Pipeline.SetMultisampingNone();
        Pipeline.DisableBlending();
        Pipeline.DisableDepthTest();
        Pipeline.SetColorAttachmentFormat(ActiveSwapChain->GetDrawImage().ImageFormat);
        Pipeline.SetDepthFormat(VK_FORMAT_UNDEFINED);

        MeshPipeline = Pipeline.BuildPipeline(Device);

        vkDestroyShaderModule(Device, FragShader, nullptr);
        vkDestroyShaderModule(Device, VertexShader, nullptr);


    }

    void FVulkanRenderContext::InitDefaultData()
    {

            testMeshes = LoadGltfMeshes("Resources/Meshes/basicmesh.glb").value();

            std::array<FVertex,4> rect_vertices;

            rect_vertices[0].Position = {0.5,-0.5, 0};
            rect_vertices[1].Position = {0.5,0.5, 0};
            rect_vertices[2].Position = {-0.5,-0.5, 0};
            rect_vertices[3].Position = {-0.5,0.5, 0};

            rect_vertices[0].Color = {0,0, 0,1};
            rect_vertices[1].Color = { 0.5,0.5,0.5 ,1};
            rect_vertices[2].Color = { 1,0, 0,1 };
            rect_vertices[3].Color = { 0,1, 0,1 };

            std::array<uint32_t,6> rect_indices;

            rect_indices[0] = 0;
            rect_indices[1] = 1;
            rect_indices[2] = 2;

            rect_indices[3] = 2;
            rect_indices[4] = 1;
            rect_indices[5] = 3;

            Rect = UploadMesh(rect_indices, rect_vertices);

    }
}
