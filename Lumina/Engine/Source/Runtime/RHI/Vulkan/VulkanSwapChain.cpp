#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "Source/Runtime/Log/Log.h"
#include <cstdint>
#include <limits>
#include <algorithm>

namespace Lumina
{

    FVulkanSwapChain::FVulkanSwapChain(VkInstance InInstance, LVulkanDevice* InDevice, GLFWwindow* InWindow)
    {
        Instance = InInstance;
        Device = InDevice;
        Window = InWindow;

    }

    void FVulkanSwapChain::Create(uint32_t* InWidth, uint32_t* InHeight, bool VSync)
    {
        VkDevice VulkDevice = Device->GetVulkanDevice();
        PhysicalDevice = Device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
        VkSwapchainKHR OldSwapChain = SwapChain;

        const FSwapChainSupportDetails SwapChainDetails = QuerySwapChainSupport(PhysicalDevice);

        VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainDetails.Formats);
        VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainDetails.PresentModes);
        VkExtent2D CachedExtent = ChooseSwapExtent(SwapChainDetails.Capabilities);

        uint32_t ImageCount = SwapChainDetails.Capabilities.minImageCount + 1; // Add one so we don't have to wait for the driver to complete internal operations.

        if (SwapChainDetails.Capabilities.maxImageCount > 0 && ImageCount > SwapChainDetails.Capabilities.maxImageCount)
        {
            ImageCount = SwapChainDetails.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        CreateInfo.surface = Surface;
        CreateInfo.minImageCount = ImageCount;
        CreateInfo.imageFormat = SurfaceFormat.format;
        CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
        CreateInfo.imageExtent = CachedExtent;
        CreateInfo.imageArrayLayers = 1;
        CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        FQueueFamilyIndices Indices = LVulkanPhysicalDevice::GetQueueFamilyIndices(PhysicalDevice);
        uint32_t QueueFamilyIndicies[] = { Indices.GraphicsFamily.value(), Indices.PresentFamily.value() };

        if(Indices.GraphicsFamily != Indices.PresentFamily)
        {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            CreateInfo.queueFamilyIndexCount = 2;
            CreateInfo.pQueueFamilyIndices = QueueFamilyIndicies;
        }
        else
        {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            CreateInfo.queueFamilyIndexCount = 0;
            CreateInfo.pQueueFamilyIndices = nullptr;
        }

        CreateInfo.preTransform = SwapChainDetails.Capabilities.currentTransform;
        CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        CreateInfo.presentMode = PresentMode;
        CreateInfo.clipped = VK_TRUE;
        CreateInfo.oldSwapchain = VK_NULL_HANDLE;

        ImageFormat = SurfaceFormat.format;
        ColorSpace = SurfaceFormat.colorSpace;
        Extent = CachedExtent;

        if(vkCreateSwapchainKHR(VulkDevice, &CreateInfo, nullptr, &SwapChain) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to create swap chain!");
            assert(0);
        }
        LE_LOG_INFO("Vulkan created swap chain successfully");
        
        vkGetSwapchainImagesKHR(VulkDevice, SwapChain, &ImageCount, nullptr);
        Images.resize(ImageCount);
        vkGetSwapchainImagesKHR(VulkDevice, SwapChain, &ImageCount, Images.data());

        CreateImageViews();
    
        RenderPass = new FVulkanRenderPass(VulkDevice, this);    
        SwapPipeline = new FVulkanPipeline(VulkDevice, this, RenderPass);
        

        FrameBuffers.resize(ImageViews.size());

        for (size_t i = 0; i < ImageViews.size(); i++)
        {
            VkImageView attachments[] =
            {
                ImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = RenderPass->GetRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = Extent.width;
            framebufferInfo.height = Extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(Device->GetVulkanDevice(), &framebufferInfo, nullptr, &FrameBuffers[i]) != VK_SUCCESS)
            {
                LE_LOG_CRITICAL("Failed to create frame buffer!");
            }
        }

        FQueueFamilyIndices PoolQueueFamilyIndicies = LVulkanPhysicalDevice::GetQueueFamilyIndices(PhysicalDevice);

        VkCommandPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        PoolInfo.queueFamilyIndex = PoolQueueFamilyIndicies.GraphicsFamily.value();

        if(vkCreateCommandPool(Device->GetVulkanDevice(), &PoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to create command pool!");
        }

        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandBufferCount = 1;

        if(vkAllocateCommandBuffers(Device->GetVulkanDevice(), &AllocInfo, &CommandBuffer) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to allocate command buffers!");
        }

        VkSemaphoreCreateInfo SemaphoreInfo = {};
        SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo FenceInfo = {};
        FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if(vkCreateSemaphore(Device->GetVulkanDevice(), &SemaphoreInfo, nullptr, &ImageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(Device->GetVulkanDevice(), &SemaphoreInfo, nullptr, &RenderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(Device->GetVulkanDevice(), &FenceInfo, nullptr, &InFlightFence) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to create semaphores!");
        }
        
    }
    
    void FVulkanSwapChain::Destroy()
    {
        vkDestroySemaphore(Device->GetVulkanDevice(), ImageAvailableSemaphore, nullptr);
        vkDestroySemaphore(Device->GetVulkanDevice(), RenderFinishedSemaphore, nullptr);
        vkDestroyFence(Device->GetVulkanDevice(), InFlightFence, nullptr);
        vkDestroyCommandPool(Device->GetVulkanDevice(), CommandPool, nullptr);
        
        for(const auto FrameBuffer : FrameBuffers)
        {
            vkDestroyFramebuffer(Device->GetVulkanDevice(), FrameBuffer, nullptr);
        }
        
        for(const auto View : ImageViews)
        {
            vkDestroyImageView(Device->GetVulkanDevice(), View, nullptr);
        }
        
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroySwapchainKHR(Device->GetVulkanDevice(), SwapChain, nullptr);
        vkDestroyInstance(Instance, nullptr);
    }

    void FVulkanSwapChain::InitSurface(GLFWwindow* InWindow)
    {
        if (glfwCreateWindowSurface(Instance, InWindow, nullptr, &Surface) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to create window surface!");
            assert(0);
        }
        LE_LOG_INFO("Initialized Window");
    }

    void FVulkanSwapChain::Present()
    {
        if(Device->GetVulkanDevice())
        {
            vkWaitForFences(Device->GetVulkanDevice(), 1, &InFlightFence, VK_TRUE, UINT64_MAX);
            vkResetFences(Device->GetVulkanDevice(), 1, &InFlightFence);

            uint32_t ImageIndex;
            vkAcquireNextImageKHR(Device->GetVulkanDevice(), SwapChain, UINT64_MAX, ImageAvailableSemaphore, VK_NULL_HANDLE, &ImageIndex);
            vkResetCommandBuffer(CommandBuffer, 0);
            vkResetCommandBuffer(CommandBuffer, 0);

            RecordCommandBuffer(CommandBuffer, ImageIndex);

            VkSubmitInfo SubmitInfo = {};
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore WaitSemaphores[] = {ImageAvailableSemaphore};
            VkPipelineStageFlags WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            
            SubmitInfo.waitSemaphoreCount = 1;
            SubmitInfo.pWaitSemaphores = WaitSemaphores;
            SubmitInfo.pWaitDstStageMask = WaitStages;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = &CommandBuffer;

            VkSemaphore SignalSemaphores[] = {RenderFinishedSemaphore};
            SubmitInfo.signalSemaphoreCount = 1;
            SubmitInfo.pSignalSemaphores = SignalSemaphores;

            if(vkQueueSubmit(Device->GetGraphicsQueue(), 1, &SubmitInfo, InFlightFence) != VK_SUCCESS)
            {
                LE_LOG_CRITICAL("Failed to submit draw command buffer!");
            }
            
            VkPresentInfoKHR PresentInfo = {};
            PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            PresentInfo.waitSemaphoreCount = 1;
            PresentInfo.pWaitSemaphores = SignalSemaphores;

            VkSwapchainKHR SwapChains[] = {SwapChain};
            PresentInfo.swapchainCount = 1;
            PresentInfo.pSwapchains = SwapChains;
            PresentInfo.pImageIndices = &ImageIndex;
            PresentInfo.pResults = nullptr;

            vkQueuePresentKHR(Device->GetComputeQueue(), &PresentInfo);
            
        }
    }

    FSwapChainSupportDetails FVulkanSwapChain::QuerySwapChainSupport(VkPhysicalDevice InDevice)
    {
        FSwapChainSupportDetails Details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(InDevice, Surface, &Details.Capabilities);

        uint32_t FormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, Surface, &FormatCount, nullptr);
        if (FormatCount != 0)
        {
            Details.Formats.resize(FormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, Surface, &FormatCount, Details.Formats.data());
        }

        uint32_t PresentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(InDevice, Surface, &PresentModeCount, nullptr);
        if (PresentModeCount != 0)
        {
            Details.PresentModes.resize(PresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(InDevice, Surface, &PresentModeCount, Details.PresentModes.data());
        }

        return Details;
    }

    VkSurfaceFormatKHR FVulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
    {
        for (const auto& AvailableFormat : AvailableFormats)
        {
            if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return AvailableFormat;
            }
        }
        return AvailableFormats[0];

    }

    VkPresentModeKHR FVulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes)
    {

        for (const auto& AvailablePresentMode : AvailablePresentModes)
        {
            if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return AvailablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D FVulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& InCapabilities) const
    {
        if (InCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return InCapabilities.currentExtent;
        }

        int Width;
        int Height;
        glfwGetFramebufferSize(Window, &Width, &Height);

        VkExtent2D ActualExtent = { static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) };

        ActualExtent.width = std::clamp(ActualExtent.width, InCapabilities.minImageExtent.width, InCapabilities.maxImageExtent.width);
        ActualExtent.height = std::clamp(ActualExtent.height, InCapabilities.minImageExtent.height, InCapabilities.maxImageExtent.height);

        return ActualExtent;
    }

    void FVulkanSwapChain::RecordCommandBuffer(VkCommandBuffer InCommandBuffer, uint32_t ImageIndex)
    {
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = 0;
        BeginInfo.pInheritanceInfo = nullptr;

        if(vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Faild to begin recording command buffer!");
        }

        VkRenderPassBeginInfo RenderPassInfo = {};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassInfo.renderPass = RenderPass->GetRenderPass();
        RenderPassInfo.framebuffer = FrameBuffers[ImageIndex];
        RenderPassInfo.renderArea.offset = {0, 0};
        RenderPassInfo.renderArea.extent = Extent;

        VkClearValue ClearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        RenderPassInfo.clearValueCount = 1;
        RenderPassInfo.pClearValues = &ClearColor;

        vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, SwapPipeline->GetPipeline());

        VkViewport Viewport{};
        Viewport.x = 0.0f;
        Viewport.y = 0.0f;
        Viewport.width = static_cast<float>(Extent.width);
        Viewport.height = static_cast<float>(Extent.height);
        Viewport.minDepth = 0.0f;
        Viewport.maxDepth = 1.0f;
        vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = Extent;
        vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);

        vkCmdDraw(CommandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(CommandBuffer);

        if(vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to record Command Buffer!");
        }


    }

    void FVulkanSwapChain::CreateImageViews()
    {
        ImageViews.resize(Images.size());

        for(size_t i = 0; i <Images.size(); i++)
        {
            VkImageViewCreateInfo CreateInfo = {};
            CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            CreateInfo.image = Images[i];
            CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            CreateInfo.format = ImageFormat;
            CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            CreateInfo.subresourceRange.baseMipLevel = 0;
            CreateInfo.subresourceRange.levelCount = 1;
            CreateInfo.subresourceRange.baseArrayLayer = 0;
            CreateInfo.subresourceRange.layerCount = 1;

            if(vkCreateImageView(Device->GetVulkanDevice(), &CreateInfo, nullptr, &ImageViews[i]) != VK_SUCCESS)
            {
                LE_LOG_CRITICAL("Failed to create image views at index {0}", i);
                assert(0);
            }
        }
        LE_LOG_INFO("Created Image Views Successfully!");
    }
}
