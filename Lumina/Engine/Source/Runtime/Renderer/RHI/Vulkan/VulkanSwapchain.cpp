#include "VulkanSwapchain.h"

#include "vk-bootstrap/src/VkBootstrap.h"
#include <glfw/glfw3.h>

#include "VulkanImage.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Core/Application.h"
#include "Core/Windows/Window.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    FVulkanSwapchain::FVulkanSwapchain(const FSwapchainSpec& InSpec)
    {
        Specifications = InSpec;
    }

    void FVulkanSwapchain::CreateSurface(const FSwapchainSpec& InSpec)
    {
        VkInstance Instance = FVulkanRenderContext::Get().GetVulkanInstance();
        
        VK_CHECK(glfwCreateWindowSurface(Instance, InSpec.Window->GetWindow(), nullptr, &Surface));
    }

    void FVulkanSwapchain::CreateSwapchain(const FSwapchainSpec& InSpec)
    {
    	Specifications = InSpec;
    	
        FVulkanRenderContext& RenderContext = FVulkanRenderContext::Get();
        auto Device = FVulkanRenderContext::GetDevice();
        
        Images.reserve(InSpec.FramesInFlight);
        Semaphores.reserve(3);
        CurrentFrameIndex = 0;

        if(Swapchain) [[likely]]
        {
            vkDestroySwapchainKHR(FVulkanRenderContext::GetDevice(), Swapchain, nullptr);
        }
    	
        vkb::SwapchainBuilder swapchainBuilder{ RenderContext.GetPhysicalDevice(), Device, Surface };

    	Format = VK_FORMAT_B8G8R8A8_UNORM;
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

        for (TRefPtr<FVulkanImage>& Image : Images)
        {
            vkDestroyImageView(Device, Image->GetImageView(), nullptr);
        }

        Images.clear();

    	VkCommandBuffer ImageCreateBuffer = FVulkanRenderContext::AllocateTransientCommandBuffer();
    	
        for (VkImage RawImage : RawImages)
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

			VK_CHECK(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView));

			FImageSpecification SwapchainImageSpec = {};
			SwapchainImageSpec.Extent = { (uint32)InSpec.Extent.x, (uint32)InSpec.Extent.y, 1 };
			SwapchainImageSpec.Usage = EImageUsage::RENDER_TARGET;
			SwapchainImageSpec.Type = EImageType::TYPE_2D;
			SwapchainImageSpec.Format = convert(SurfaceFormat.format);

			Images.push_back(MakeRefPtr<FVulkanImage>(SwapchainImageSpec, RawImage, ImageView));

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

            VK_CHECK(vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &RenderSemaphore));
            VK_CHECK(vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &PresentSemaphore));

            Semaphores.emplace_back(RenderSemaphore, PresentSemaphore);
        }
        Semaphores.shrink_to_fit();


        Fences.shrink_to_fit();
        Fences.reserve(InSpec.FramesInFlight);

        
        VkFenceCreateInfo FenceCreateInfo = {};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < InSpec.FramesInFlight; i++)
        {
            VkFence Fence;
            VK_CHECK(vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence));
            Fences.push_back(Fence);
        }
        
    }

    void FVulkanSwapchain::CreateImages()
    {
    }

    void FVulkanSwapchain::DestroySurface()
    {
    	vkDestroySurfaceKHR(FVulkanRenderContext::GetVulkanInstance(), Surface, nullptr);
    }

    void FVulkanSwapchain::DestroySwapchain()
    {
    	auto Device = FVulkanRenderContext::GetDevice();

    	VK_CHECK(vkDeviceWaitIdle(Device));
    	for (auto& Image : Images)
    	{
    		vkDestroyImageView(Device, Image->GetImageView(), nullptr);
    	}

    	vkDestroySwapchainKHR(Device, Swapchain, nullptr);

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

    void FVulkanSwapchain::RecreateSwapchain()
    {
    	uint32 Height = FApplication::GetWindow().GetHeight();
    	uint32 Width = FApplication::GetWindow().GetWidth();
    	GetSpecs().Extent = {Width, Height};
    	
    	vkQueueWaitIdle(FVulkanRenderContext::GetGeneralQueue());
    	FRenderer::WaitIdle();
		CreateSwapchain(GetSpecs());
    	FRenderer::WaitIdle();
    	bDirty = false;
    }

    bool FVulkanSwapchain::BeginFrame()
    {
        auto Device = FVulkanRenderContext::GetDevice();
    	
    	if(Swapchain == VK_NULL_HANDLE)
    	{
    		LOG_WARN("Attempted to begin swap chain frame before swap chain creation");
    		return false;
    	}
    	
    	VK_CHECK(vkWaitForFences(Device,  1, &Fences[CurrentFrameIndex], VK_TRUE, UINT64_MAX));
        
        VkResult AcquireResult = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, Semaphores[CurrentFrameIndex].Present,
            VK_NULL_HANDLE, &CurrentImageIndex);

        if (AcquireResult == VK_ERROR_OUT_OF_DATE_KHR || AcquireResult == VK_SUBOPTIMAL_KHR || bDirty)
        {
        	bDirty = true;
        	return false;
        }
    	
    	VK_CHECK(vkResetFences(Device, 1, &Fences[CurrentFrameIndex]));
        return true;
    }

    void FVulkanSwapchain::EndFrame()
    {
        if(Swapchain == VK_NULL_HANDLE || bDirty)
        {
	        return;
        }
    	
        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.pImageIndices = &CurrentImageIndex;
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = &Swapchain;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &Semaphores[CurrentFrameIndex].Render;
        PresentInfo.pResults = nullptr;

    	VkQueue Queue = FVulkanRenderContext::GetGeneralQueue();
    	assert(Queue != VK_NULL_HANDLE);
    	
        VkResult Result = vkQueuePresentKHR(Queue, &PresentInfo);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || bDirty)
        {
        	bDirty = true;
        }
    	
        CurrentFrameIndex = (CurrentImageIndex + 1) % Specifications.FramesInFlight;
    }
}