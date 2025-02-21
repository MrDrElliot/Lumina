#include "VulkanSwapchain.h"

#include "vk-bootstrap/src/VkBootstrap.h"
#include <glfw/glfw3.h>
#include "VulkanImage.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Core/Application/Application.h"
#include "Core/Windows/Window.h"
#include "Platform/Platform.h"
#include "Renderer/RHIIncl.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    FVulkanSwapchain::FVulkanSwapchain(const FSwapchainSpec& InSpec)
		: Format()
		, SurfaceFormat()
		, CurrentPresentMode()
    {
	    Specifications = InSpec;
	    bWasResizedThisFrame = false;
    	
    	FMemory::MemsetZero(AquireSemaphores.data(), AquireSemaphores.size() * sizeof(VkSemaphore));
    	FMemory::MemsetZero(PresentSemaphores.data(), PresentSemaphores.size() * sizeof(VkSemaphore));
    }

    void FVulkanSwapchain::CreateSurface(IRenderContext* Context, const FSwapchainSpec& InSpec)
    {
    	FVulkanRenderContext* RenderContext = static_cast<FVulkanRenderContext*>(Context);
        VkInstance Instance = RenderContext->GetVulkanInstance();
        
        VK_CHECK(glfwCreateWindowSurface(Instance, InSpec.Window->GetWindow(), nullptr, &Surface));
    }

    void FVulkanSwapchain::CreateSwapchain(IRenderContext* Context, const FSwapchainSpec& InSpec)
    {
    	Specifications = InSpec;
    	
    	FVulkanRenderContext* RenderContext = static_cast<FVulkanRenderContext*>(Context);
        VkDevice Device = RenderContext->GetDevice();
    	
        Images.reserve(FRAMES_IN_FLIGHT);

    	bool bBeingResized = false;
        if(LIKELY(Swapchain))
        {
        	FRenderer::WaitIdle();
            vkDestroySwapchainKHR(Device, Swapchain, nullptr);
        	bBeingResized = true;
        }
    	
        vkb::SwapchainBuilder swapchainBuilder { RenderContext->GetPhysicalDevice(), Device, Surface };

    	Format = VK_FORMAT_B8G8R8A8_UNORM;
        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    	SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(SurfaceFormat)
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_min_image_count(FRAMES_IN_FLIGHT)
            .set_image_array_layer_count(1)
            .set_desired_extent(InSpec.Extent.X, InSpec.Extent.Y)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build()
            .value();

        Swapchain = vkbSwapchain.swapchain;


        std::vector<VkImage> RawImages = vkbSwapchain.get_images().value();
    	
        Images.clear();

    	VkCommandBuffer ImageCreateBuffer = RenderContext->AllocateTransientCommandBuffer();
    	
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
			SwapchainImageSpec.Extent = FVector3D(InSpec.Extent.X, InSpec.Extent.Y, 1.0f);
			SwapchainImageSpec.Usage = EImageUsage::RENDER_TARGET;
			SwapchainImageSpec.Type = EImageType::TYPE_2D;
			SwapchainImageSpec.Format = convert(SurfaceFormat.format);

			Images.push_back(FRHIImage(MakeRefCount<FVulkanImage>(SwapchainImageSpec, RawImage, ImageView)));

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

    	RenderContext->ExecuteTransientCommandBuffer(ImageCreateBuffer);

    	size_t currentImageCount = RawImages.size();

    	// Destroy excess PresentSemaphores and resize down
    	if (PresentSemaphores.size() > currentImageCount)
    	{
    		for (size_t i = currentImageCount; i < PresentSemaphores.size(); i++)
    		{
    			PresentSemaphores[i].SafeRelease();
    		}

    		// Now safely resize down
    		PresentSemaphores.resize(currentImageCount);
    	}

    	// Ensure PresentSemaphores has enough slots (initialize with null)
    	if (PresentSemaphores.size() < currentImageCount)
    	{
    		PresentSemaphores.clear();
    		PresentSemaphores.resize(currentImageCount);
    	}

    	// Create new semaphores
    	for (size_t i = 0; i < currentImageCount; i++)
    	{
    		PresentSemaphores[i] = MakeRefCount<FVulkanSemaphore>();
    		PresentSemaphores[i]->SetFriendlyName("Present Semaphore: " + eastl::to_string(i));
    	}

    	// --------- AQUIRE SEMAPHORES MANAGEMENT --------- //
    	
    	// Destroy excess AquireSemaphores
    	if (AquireSemaphores.size() > FRAMES_IN_FLIGHT)
    	{
    		for (size_t i = FRAMES_IN_FLIGHT; i < AquireSemaphores.size(); i++)
    		{
    			AquireSemaphores[i].SafeRelease();
    		}
    		
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Ensure AquireSemaphores has enough slots
    	if (AquireSemaphores.size() < FRAMES_IN_FLIGHT)
    	{
    		AquireSemaphores.clear();
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Create new Aquire semaphores
    	for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    	{
    		AquireSemaphores[i] = MakeRefCount<FVulkanSemaphore>();
    		AquireSemaphores[i]->SetFriendlyName("Aquire Semaphore: " + eastl::to_string(i));
    	}

    	
    	if(bWasResizedThisFrame)
    	{
    		return;
    	}
    	
        VkFenceCreateInfo FenceCreateInfo = {};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        Fences.reserve(FRAMES_IN_FLIGHT);
        for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
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
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	VkInstance Instance = RenderContext->GetVulkanInstance();
    	
    	vkDestroySurfaceKHR(Instance, Surface, nullptr);
    }

    void FVulkanSwapchain::DestroySwapchain()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();

    	auto Device = RenderContext->GetDevice();
		FRenderer::WaitIdle();
    	
    	Images.clear();
    	
    	vkDestroySwapchainKHR(Device, Swapchain, nullptr);
    	
    	AquireSemaphores.clear();
    	PresentSemaphores.clear();
    	
	    for (int i = 0; i < Fences.size(); ++i)
	    {
		    vkDestroyFence(Device, Fences[i], nullptr);
	    }
    	
    	Fences.clear();

    	Swapchain = VK_NULL_HANDLE;
    }

    void FVulkanSwapchain::RecreateSwapchain()
    {
    	LOG_WARN("Re-sizing Swapchain");
    	uint32 Height = FApplication::Get().GetMainWindow()->GetHeight();
    	uint32 Width = FApplication::Get().GetMainWindow()->GetWidth();
    	GetSpecs().Extent = FVector2D(Height, Width);

    	FRenderer::WaitIdle();
		CreateSwapchain(FRenderer::GetRenderContext(), GetSpecs());
    	
    	bWasResizedThisFrame = true;
    	bDirty = false;
    }

    bool FVulkanSwapchain::BeginFrame()
    {
    	bWasResizedThisFrame = false;
    	if(UNLIKELY(bDirty))
    	{
    		RecreateSwapchain();
    	}
    	
        VkDevice Device = FRenderer::GetRenderContext<FVulkanRenderContext>()->GetDevice();
    	
    	if(UNLIKELY(Swapchain == VK_NULL_HANDLE))
    	{
    		LOG_WARN("Attempted to begin swap chain frame before swap chain creation");
    		return false;
    	}
    	
    	VK_CHECK(vkWaitForFences(Device,  1, &Fences[CurrentFrameIndex], VK_TRUE, UINT64_MAX));
        
        VkResult AcquireResult = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, GetAquireSemaphore(),
            VK_NULL_HANDLE, &CurrentImageIndex);

        if (UNLIKELY(AcquireResult == VK_ERROR_OUT_OF_DATE_KHR || AcquireResult == VK_SUBOPTIMAL_KHR))
        {
        	bDirty = true;
        }
    	
    	VK_CHECK(vkResetFences(Device, 1, &Fences[CurrentFrameIndex]));
        return true;
    }

    void FVulkanSwapchain::Present()
    {
        if(Swapchain == VK_NULL_HANDLE || bDirty)
        {
	        return;
        }

        VkSemaphore PresentSemaphore = GetPresentSemaphore();
    	
        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.pImageIndices = &CurrentImageIndex;
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = &Swapchain;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &PresentSemaphore;
        PresentInfo.pResults = nullptr;

    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	
    	VkQueue Queue = RenderContext->GetGeneralQueue();
        VkResult Result = vkQueuePresentKHR(Queue, &PresentInfo);

        if (UNLIKELY(Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR))
        {
        	bDirty = true;
        }
        else
        {
	        VK_CHECK(Result);
        }
    	
        CurrentFrameIndex = (CurrentFrameIndex + 1) % FRAMES_IN_FLIGHT;
    }
}
