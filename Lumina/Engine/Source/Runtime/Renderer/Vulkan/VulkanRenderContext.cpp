#include "VulkanRenderContext.h"
#include <imgui/imgui.h>
#include <vk-bootstrap/src/VkBootstrap.h>
#include "VulkanHelpers.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

#include <chrono>

#include "Source/Runtime/Assets/StaticMesh/StaticMesh.h"
#include "Source/Runtime/Scene/Scene.h"

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

        VkCommandBuffer Cmd = GetCurrentFrame().CommandBuffer;

        uint32_t SwapChainImageIndex = 0;
        if(BeginFrame(Cmd, &SwapChainImageIndex) == false) return;


        FAllocatedImage DrawImage = ActiveSwapChain->GetDrawImage();
        FAllocatedImage DepthImage = ActiveSwapChain->GetDepthImage();

        
        Vulkan::TransitionImage(Cmd, DrawImage,  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        Vulkan::TransitionImage(Cmd, DepthImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        /* Geometry Render */
        DrawGeometry(Cmd);

        Vulkan::TransitionImage(Cmd, DrawImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        Vulkan::CopyImageToImage(Cmd, DrawImage, ActiveSwapChain->GetImages()[SwapChainImageIndex], ActiveSwapChain->GetDrawExtent2D(), ActiveSwapChain->GetExtent2D());

        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        Vulkan::TransitionImage(Cmd, DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        /* ImGui Render */
        DrawImGui(Cmd, ActiveSwapChain->GetImageViews()[SwapChainImageIndex]);
        
        
        // Set swapchain image layout to Present so we can show it on the screen
        Vulkan::TransitionImage(Cmd, ActiveSwapChain->GetImages()[SwapChainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        
        SubmitFrame(Cmd, SwapChainImageIndex);
        
        FrameNumber++;
    }

    void FVulkanRenderContext::DrawGeometry(VkCommandBuffer InCmd)
    {

        VkClearValue ClearValue = {};
        ClearValue.color = {{0.0f}};
        VkRenderingAttachmentInfo colorAttachment = Vulkan::RenderingAttachmentInfo(ActiveSwapChain->GetDrawImage().ImageView, &ClearValue, VK_IMAGE_LAYOUT_GENERAL);
        VkRenderingAttachmentInfo depthAttachment = Vulkan::DepthAttachmentInfo(ActiveSwapChain->GetDepthImage().ImageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
        
        VkRenderingInfo renderInfo = Vulkan::RenderingInfo(ActiveSwapChain->GetDrawExtent2D(), &colorAttachment, &depthAttachment);
        if(renderInfo.renderArea.extent.width == 0 || renderInfo.renderArea.extent.height == 0) return;

        
        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(ActiveSwapChain->GetDrawExtent2D().width);
        viewport.height = static_cast<float>(ActiveSwapChain->GetDrawExtent2D().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(InCmd, 0, 1, &viewport);


        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = ActiveSwapChain->GetDrawExtent2D().width;
        scissor.extent.height = ActiveSwapChain->GetDrawExtent2D().height;
        vkCmdSetScissor(InCmd, 0, 1, &scissor);

        
        vkCmdBeginRendering(InCmd, &renderInfo);

        vkCmdBindPipeline(InCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, MeshPipeline);

        VkDescriptorSet imageSet = GetCurrentFrame().FrameDescriptors.Allocate(Device, SingleImageDescriptorLayout);
        FDescriptorWriter writer;
        writer.WriteImage(0, ErrorCheckerboardImage.ImageView, DefaultSamplerNearest, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.UpdateSet(Device, imageSet);

        

        vkCmdBindDescriptorSets(InCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, MeshPipelineLayout, 0, 1, &imageSet, 0, nullptr);
        
        
        
        vkCmdPushConstants(InCmd, MeshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FGPUDrawPushConstants), &PushConstants);
        vkCmdBindIndexBuffer(InCmd, testMeshes[2]->GetMeshBuffers().IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(InCmd, testMeshes[2]->GetSurfaces()[0].Count, 1, testMeshes[2]->GetSurfaces()[0].StartIndex, 0, 0);
        
        vkCmdEndRendering(InCmd);
        
    }
    

    void FVulkanRenderContext:: DrawImGui(VkCommandBuffer InBuffer, VkImageView TargetViewImage)
    {
        if (ImDrawData* DrawData = ImGui::GetDrawData())
        {
            VkRenderingAttachmentInfo ColorAttachment = Vulkan::RenderingAttachmentInfo(TargetViewImage, nullptr, VK_IMAGE_LAYOUT_GENERAL);
            VkRenderingInfo RenderInfo = Vulkan::RenderingInfo(ActiveSwapChain->GetExtent2D(), &ColorAttachment, nullptr);

            vkCmdBeginRendering(InBuffer, &RenderInfo);
            
            ImGui_ImplVulkan_RenderDrawData(DrawData, InBuffer);

            vkCmdEndRendering(InBuffer);
        }
    }

    bool FVulkanRenderContext::BeginFrame(VkCommandBuffer InCmd, uint32_t* InSwapChainImageIndex)
    {
        
        /* Wait until the GPU has finished rendering the previous frame */
        VK_CHECK(vkWaitForFences(Device, 1, &GetCurrentFrame().RenderFence, true, 1000000000));

        GetCurrentFrame().DeletionQueue.Flush();
        GetCurrentFrame().FrameDescriptors.ClearPools(Device);
        
        
        /* Request the rendered image from the swap chain */
        VkResult ImageResult = vkAcquireNextImageKHR(Device, ActiveSwapChain->GetSwapChain(), 1000000000, GetCurrentFrame().SwapchainSemaphore, nullptr, InSwapChainImageIndex);
        if(ImageResult == VK_ERROR_OUT_OF_DATE_KHR || ImageResult == VK_SUBOPTIMAL_KHR)
        {
            ActiveSwapChain->SetResizeRequested(true);
            return false;
        }

        VK_CHECK(vkResetFences(Device, 1, &GetCurrentFrame().RenderFence));

        
        
        
        /* Reset the commad buffer since we know it's finished, we need to do this to start recording again */
        VK_CHECK(vkResetCommandBuffer(InCmd, 0));
        

        /* Begin recording to the command buffer of this frame */
        VkCommandBufferBeginInfo CmdBeginInfo = Vulkan::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        VK_CHECK(vkBeginCommandBuffer(InCmd, &CmdBeginInfo));

        return true;
    }

    void FVulkanRenderContext::SubmitFrame(VkCommandBuffer InCmd, uint32_t SwapChainImageIndex)
    {
        
        /* End the command buffer, commands cannot be added to it after this point */
        VK_CHECK(vkEndCommandBuffer(InCmd));

        
        VkCommandBufferSubmitInfo CmdInfo = Vulkan::CommandBufferSubmitInfo(InCmd);
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


        VkResult PresentResult = vkQueuePresentKHR(GraphicsQueue, &PresentInfo);
        if(PresentResult == VK_ERROR_OUT_OF_DATE_KHR || PresentResult == VK_SUBOPTIMAL_KHR)
        {
            ActiveSwapChain->SetResizeRequested(true);
        }
        
    }

    FAllocatedImage FVulkanRenderContext::CreateImage(VkExtent3D InSize, VkFormat InFormat, VkImageUsageFlags InUsage, bool bMipmapped)
    {
        FAllocatedImage newImage;
        newImage.ImageFormat = InFormat;
        newImage.ImageExtent = InSize;

        VkImageCreateInfo img_info = Vulkan::ImageCreateInfo(InFormat, InUsage, InSize);
        if (bMipmapped)
        {
            img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(InSize.width, InSize.height)))) + 1;
        }

        // always allocate images on dedicated GPU memory
        VmaAllocationCreateInfo allocinfo = {};
        allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocinfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // allocate and create the image
        VK_CHECK(vmaCreateImage(Allocator, &img_info, &allocinfo, &newImage.Image, &newImage.Allocation, nullptr));

        // if the format is a depth format, we will need to have it use the correct
        // aspect flag
        VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
        if (InFormat == VK_FORMAT_D32_SFLOAT)
        {
            aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        // build a image-view for the image
        VkImageViewCreateInfo view_info = Vulkan::ImageViewCreateInfo(InFormat, newImage.Image, aspectFlag);
        view_info.subresourceRange.levelCount = img_info.mipLevels;

        VK_CHECK(vkCreateImageView(Device, &view_info, nullptr, &newImage.ImageView));

        return newImage;
    }

    FAllocatedImage FVulkanRenderContext::CreateImage(void* InData, VkExtent3D InSize, VkFormat InFormat, VkImageUsageFlags InUsage, bool bMipmapped)
    {
        size_t data_size = InSize.depth * InSize.width * InSize.height * 4;
        FAllocatedBuffer uploadbuffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        memcpy(uploadbuffer.Info.pMappedData, InData, data_size);

        FAllocatedImage Newimage = CreateImage(InSize, InFormat, InUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, bMipmapped);

        ImmediateSubmit([&](VkCommandBuffer cmd)
        {
            Vulkan::TransitionImage(cmd, Newimage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkBufferImageCopy copyRegion = {};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;

            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = InSize;

            // copy the buffer into the image
            vkCmdCopyBufferToImage(cmd, uploadbuffer.Buffer, Newimage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            Vulkan::TransitionImage(cmd, Newimage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); });

        DestroyBuffer(uploadbuffer);

        return Newimage;
    }

    void FVulkanRenderContext::DestroyImage(const FAllocatedImage& InImage)
    {
        vkDestroyImageView(Device, InImage.ImageView, nullptr);
        vmaDestroyImage(Allocator, InImage.Image, InImage.Allocation);
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
        newSurface.VertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

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

        ImmediateSubmit([&](VkCommandBuffer cmd)
        {
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

        /* Initialize ImGuiLayer */
        FApplication::Get().InitImGuiLayer();
        
        /* Initialize Default Data */
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
        std::vector<FDescriptorAllocatorGrowable::FPoolSizeRatio> Sizes =
            { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
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

        
        GlobalDescriptorAllocator.Init(Device, 1000, Sizes);

        FDescriptorLayoutBuilder LayoutBuilder;
        LayoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        DrawImageDescriptorLayout = LayoutBuilder.Build(Device, VK_SHADER_STAGE_COMPUTE_BIT);

        DrawImageDescriptors = GlobalDescriptorAllocator.Allocate(Device, DrawImageDescriptorLayout);

        FDescriptorWriter Writer;
        Writer.WriteImage(0, ActiveSwapChain->GetDrawImage().ImageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        Writer.UpdateSet(Device, DrawImageDescriptors);

        FDescriptorLayoutBuilder Builder;
        Builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        GpuSceneDataDescriptorLayout = Builder.Build(Device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

        FDescriptorLayoutBuilder Builder2;
        Builder2.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        SingleImageDescriptorLayout = Builder2.Build(Device, VK_SHADER_STAGE_FRAGMENT_BIT);
        

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            std::vector<FDescriptorAllocatorGrowable::FPoolSizeRatio> frame_sizes =
            { 
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
            };

            Frames[i].FrameDescriptors = FDescriptorAllocatorGrowable{};
            Frames[i].FrameDescriptors.Init(Device, 1000, frame_sizes);
	
            MainDeletionQueue.Add([&, i]()
            {
                Frames[i].FrameDescriptors.DestroyPools(Device);
            });
        }
    }
    
    void FVulkanRenderContext::InitPipelines()
    {
        InitMeshPipeline();

        metalRoughMaterial.BuildPipelines();
    }
    

    void FVulkanRenderContext::InitMeshPipeline()
    {
        VkShaderModule FragShader;
        if (FragShader = Vulkan::LoadShaderModule("../Lumina/Engine/Resources/Shaders/tex_image.frag.spv", Device); FragShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create colored triangle fragment shader!");
        }
        
        VkShaderModule VertexShader;
        if (VertexShader = Vulkan::LoadShaderModule("../Lumina/Engine/Resources/Shaders/Colored_Triangle_mesh.vert.spv", Device); VertexShader == nullptr)
        {
            LE_LOG_CRITICAL("Failed to create triangle mesh vertex shader!");
        }


        VkPushConstantRange bufferRange{};
        bufferRange.offset = 0;
        bufferRange.size = sizeof(FGPUDrawPushConstants);
        bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipeline_layout_info = Vulkan::PipelineLayoutCreateInfo();
        pipeline_layout_info.pPushConstantRanges = &bufferRange;
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pSetLayouts = &SingleImageDescriptorLayout;
        pipeline_layout_info.setLayoutCount = 1;
        VK_CHECK(vkCreatePipelineLayout(Device, &pipeline_layout_info, nullptr, &MeshPipelineLayout));

        
        FVulkanPipeline Pipeline;
        Pipeline.PipelineLayout = MeshPipelineLayout;
        Pipeline.SetShaders(VertexShader, FragShader);
        Pipeline.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        Pipeline.SetPolygonMode(VK_POLYGON_MODE_FILL);
        Pipeline.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        Pipeline.SetMultisampingNone();
        // Pipeline.DisableBlending();
        Pipeline.EnableBlendingAdditive();
        Pipeline.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
        Pipeline.SetColorAttachmentFormat(ActiveSwapChain->GetDrawImage().ImageFormat);
        Pipeline.SetDepthFormat(ActiveSwapChain->GetDepthImage().ImageFormat);

        MeshPipeline = Pipeline.BuildPipeline(Device);

        vkDestroyShaderModule(Device, FragShader, nullptr);
        vkDestroyShaderModule(Device, VertexShader, nullptr);


    }

    void FVulkanRenderContext::InitDefaultData()
    {
        testMeshes = LoadGltfMeshes("../Lumina/Engine/Resources/Meshes/basicmesh.glb").value();

        constexpr uint32_t white = std::byteswap(0xFFFFFFFF);
        WhiteImage = CreateImage((void*)&white, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        constexpr uint32_t grey = std::byteswap(0xAAAAAAFF);
        GreyImage = CreateImage((void*)&grey, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        constexpr uint32_t black = std::byteswap(0x000000FF);
        BlackImage = CreateImage((void*)&black, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        constexpr uint32_t magenta = std::byteswap(0xFF00FFFF);
        std::array<unsigned, 16 * 16 > pixels;
        
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                pixels[y*16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }

        
        ErrorCheckerboardImage = CreateImage(pixels.data(), VkExtent3D{16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        VkSamplerCreateInfo sampl = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

        sampl.magFilter = VK_FILTER_NEAREST;
        sampl.minFilter = VK_FILTER_NEAREST;

        vkCreateSampler(Device, &sampl, nullptr, &DefaultSamplerNearest);

        sampl.magFilter = VK_FILTER_LINEAR;
        sampl.minFilter = VK_FILTER_LINEAR;
        vkCreateSampler(Device, &sampl, nullptr, &DefaultSamplerLinear);

        GetActiveSwapChain()->GetDrawImage().ImGuiTexture = ImGui_ImplVulkan_AddTexture(DefaultSamplerLinear, GetActiveSwapChain()->GetDrawImage().ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        GLTFMetallicRoughness::Resources materialResources;
        materialResources.ColorImage = WhiteImage;
        materialResources.ColorSampler = DefaultSamplerLinear;
        materialResources.MetalRoughImage = WhiteImage;
        materialResources.MetalRoughSampler = DefaultSamplerLinear;

        FAllocatedBuffer materialConstants = CreateBuffer(sizeof(GLTFMetallicRoughness::Constants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);


        VmaAllocationInfo Info;
        vmaGetAllocationInfo(Allocator, materialConstants.Allocation, &Info);
        void* Data = Info.pMappedData;

        auto* sceneUniformData = static_cast<GLTFMetallicRoughness::Constants*>(Data);
        sceneUniformData->Color = glm::vec4{1,1,1,1};
        sceneUniformData->MetalRoughness = glm::vec4{1,0.5,0,0};

        MainDeletionQueue.Add([&]()
        {
            DestroyBuffer(materialConstants);
        });

        materialResources.DataBuffer = materialConstants.Buffer;
        materialResources.DataBufferOffset = 0;

        defaultData = metalRoughMaterial.WriteMaterial(Device, EMaterialPass::Transparent, materialResources, GlobalDescriptorAllocator);
        
    }
}
