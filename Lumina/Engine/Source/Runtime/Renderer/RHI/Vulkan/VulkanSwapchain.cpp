#include "VulkanSwapchain.h"

#include "VkBootstrap.h"
#include <glfw/glfw3.h>

#include "VulkanImage.h"
#include "VulkanRenderContext.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"

namespace Lumina
{
    FVulkanSwapchain::FVulkanSwapchain(const FSwapchainSpec& InSpec)
    {
        Specifications = InSpec;
    	
    }

    void FVulkanSwapchain::CreateSurface(const FSwapchainSpec& InSpec)
    {
        VkInstance Instance = FVulkanRenderContext::Get().GetVulkanInstance();
        
        glfwCreateWindowSurface(Instance, InSpec.Window->GetWindow(), nullptr, &Surface);
    }

    void FVulkanSwapchain::CreateSwapchain(const FSwapchainSpec& InSpec)
    {

        FVulkanRenderContext& RenderContext = FVulkanRenderContext::Get();
        auto Device = FVulkanRenderContext::GetDevice();
        
        Images.reserve(InSpec.FramesInFlight);
        Semaphores.reserve(3);
        CurrentFrameIndex = 0;

        if(Swapchain.has_value())
        {
            vkDestroySwapchainKHR(FVulkanRenderContext::GetDevice(), Swapchain.value(), nullptr);
        }
        
        
        vkb::SwapchainBuilder swapchainBuilder{ RenderContext.GetPhysicalDevice(), Device, Surface };

        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    	SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(SurfaceFormat)
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_min_image_count(InSpec.FramesInFlight)
            .set_image_array_layer_count(1)
            .set_desired_extent(InSpec.Extent.x, InSpec.Extent.y)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build()
            .value();

        Swapchain = vkbSwapchain.swapchain;


        std::vector<VkImage> RawImages = vkbSwapchain.get_images().value();

        for (auto Image : Images)
        {
            vkDestroyImageView(Device, Image->GetImageView(), nullptr);
        }

        Images.clear();

    	VkCommandBuffer ImageCreateBuffer = FVulkanRenderContext::AllocateTransientCommandBuffer();
    	
        for (auto RawImage : RawImages)
        {
        	VkImageView ImageView = VK_NULL_HANDLE;
        	
            VkImageViewCreateInfo ImageViewCreateInfo = {};
			ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ImageViewCreateInfo.image = RawImage;
			ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ImageViewCreateInfo.format = SurfaceFormat.format;
			ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			ImageViewCreateInfo.subresourceRange.layerCount = 1;
			ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			ImageViewCreateInfo.subresourceRange.levelCount = 1;

			vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView);

			FImageSpecification SwapchainImageSpec = {};
			SwapchainImageSpec.Extent = { (glm::uint32)InSpec.Extent.x, (glm::uint32)InSpec.Extent.y, 1 };
			SwapchainImageSpec.Usage = EImageUsage::RENDER_TARGET;
			SwapchainImageSpec.Type = EImageType::TYPE_2D;
			SwapchainImageSpec.Format = convert(SurfaceFormat.format);

			Images.push_back(std::make_shared<FVulkanImage>(SwapchainImageSpec, RawImage, ImageView));

			VkImageMemoryBarrier ImageMemoryBarrier = {};
			ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			ImageMemoryBarrier.image = RawImage;
			ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
			ImageMemoryBarrier.subresourceRange.layerCount = 1;
			ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
			ImageMemoryBarrier.subresourceRange.levelCount = 1;

			vkCmdPipelineBarrier(
				ImageCreateBuffer,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&ImageMemoryBarrier
			);
        }

    	FVulkanRenderContext::ExecuteTransientCommandBuffer(ImageCreateBuffer);

        VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (glm::int32 i = 0; i < InSpec.FramesInFlight; i++)
        {
            VkSemaphore RenderSemaphore;
            VkSemaphore PresentSemaphore;

            vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &RenderSemaphore);
            vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &PresentSemaphore);

            Semaphores.push_back({ RenderSemaphore, PresentSemaphore });
        }
        Semaphores.shrink_to_fit();
        

        Fences.reserve(InSpec.FramesInFlight);
        Fences.shrink_to_fit();

        
        VkFenceCreateInfo FenceCreateInfo = {};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (glm::int32 i = 0; i < InSpec.FramesInFlight; i++)
        {
            VkFence Fence;
            vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence);
            Fences.push_back(Fence);
        }
        
    }

    void FVulkanSwapchain::DestroySurface()
    {
    	vkDestroySurfaceKHR(FVulkanRenderContext::GetVulkanInstance(), Surface, nullptr);
    }

    void FVulkanSwapchain::DestroySwapchain()
    {
    	auto Device = FVulkanRenderContext::GetDevice();

    	vkDeviceWaitIdle(Device);
    	for (auto& Image : Images)
    	{
    		vkDestroyImageView(Device, Image->GetImageView(), nullptr);
    	}

    	vkDestroySwapchainKHR(Device, Swapchain.value(), nullptr);

    	for (auto& Semaphore : Semaphores)
    	{
    		vkDestroySemaphore(Device, Semaphore.Render, nullptr);
    		vkDestroySemaphore(Device, Semaphore.Present, nullptr);
    	}

    	for (auto& Fence : Fences)
    	{
    		vkDestroyFence(Device, Fence, nullptr);
    	}

    	Swapchain = VK_NULL_HANDLE;
    }

    void FVulkanSwapchain::BeginFrame()
    {
        auto Device = FVulkanRenderContext::GetDevice();

        vkWaitForFences(Device,  1, &Fences[CurrentFrameIndex], VK_TRUE, UINT64_MAX);
        vkResetFences(Device, 1, &Fences[CurrentFrameIndex]);
        
        VkResult AcquireResult = vkAcquireNextImageKHR(
            Device,
            Swapchain.value(),
            UINT64_MAX,
            Semaphores[CurrentFrameIndex].Present,
            VK_NULL_HANDLE,
            &CurrentImageIndex
        );

        if (AcquireResult == VK_ERROR_OUT_OF_DATE_KHR || AcquireResult == VK_SUBOPTIMAL_KHR)
        {
            VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(FVulkanRenderContext::GetPhysicalDevice(), Surface, &SurfaceCapabilities);

            FSwapchainSpec NewSpec = GetSpecs();
            NewSpec.Extent = { (glm::int32)SurfaceCapabilities.currentExtent.width, (glm::int32)SurfaceCapabilities.currentExtent.height };

            vkQueueWaitIdle(FVulkanRenderContext::GetGeneralQueue());

            CreateSwapchain(NewSpec);
        }
    }

    void FVulkanSwapchain::EndFrame()
    {
        
        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.pImageIndices = &CurrentImageIndex;
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = &Swapchain.value();
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &Semaphores[CurrentFrameIndex].Render;
        PresentInfo.pResults = nullptr;

        VkResult present_result = vkQueuePresentKHR(FVulkanRenderContext::GetGeneralQueue(), &PresentInfo);

        if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR)
        {
            VkSurfaceCapabilitiesKHR surface_capabilities = {};
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(FVulkanRenderContext::GetPhysicalDevice(), Surface, &surface_capabilities);

            FSwapchainSpec NewSpec = GetSpecs();
            NewSpec.Extent = { (glm::int32)surface_capabilities.currentExtent.width, (glm::int32)surface_capabilities.currentExtent.height };

            vkQueueWaitIdle(FVulkanRenderContext::GetGeneralQueue());

            CreateSwapchain(NewSpec);
        }
		
		
        CurrentFrameIndex = (CurrentImageIndex + 1) % Specifications.FramesInFlight;
    }
}
