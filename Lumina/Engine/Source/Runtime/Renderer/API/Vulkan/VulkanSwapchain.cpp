
#include "VulkanBarriers.h"
#include "VulkanCommandList.h"
#include "VulkanDevice.h"
#include "Renderer/CommandList.h"
#include "..\..\StateTracking.h"
#ifdef LUMINA_RENDERER_VULKAN

#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Core/Windows/Window.h"
#include "Renderer/RenderTypes.h"
#include "src/VkBootstrap.h"

#include "VulkanSwapchain.h"
#include <glfw/glfw3.h>

namespace Lumina
{
    FVulkanSwapchain::~FVulkanSwapchain()
    {
    	
    	vkDestroySwapchainKHR(Context->GetDevice()->GetDevice(), Swapchain, nullptr);

    	SwapchainImages.clear();
    	
    	for (VkFence Fence : Fences)
    	{
    		vkDestroyFence(Context->GetDevice()->GetDevice(), Fence, nullptr);
    	}

    	for (VkSemaphore Semaphore : AquireSemaphores)
    	{
    		vkDestroySemaphore(Context->GetDevice()->GetDevice(), Semaphore, nullptr);    
    	}

    	for (VkSemaphore Semaphore : PresentSemaphores)
    	{
    		vkDestroySemaphore(Context->GetDevice()->GetDevice(), Semaphore, nullptr);    
    	}

    	
    	vkDestroySurfaceKHR(Context->GetVulkanInstance(), Surface, nullptr);
    	
    	Fences.clear();
    	AquireSemaphores.clear();
    	PresentSemaphores.clear();
    }

    void FVulkanSwapchain::CreateSwapchain(VkInstance Instance, FVulkanRenderContext* InContext, FWindow* Window, FIntVector2D Extent, bool bFromResize)
    {
    	Context = InContext;
    	SwapchainExtent = Extent;

    	if (bFromResize == false)
    	{
			VK_CHECK(glfwCreateWindowSurface(Instance, Window->GetWindow(), nullptr, &Surface));
    	}
    	
        SwapchainImages.reserve(FRAMES_IN_FLIGHT);
    	
    	
        vkb::SwapchainBuilder swapchainBuilder { Context->GetDevice()->GetPhysicalDevice(), Context->GetDevice()->GetDevice(), Surface };

    	Format = VK_FORMAT_B8G8R8A8_UNORM;
        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    	SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(SurfaceFormat)
            .set_desired_present_mode(CurrentPresentMode)
            .set_desired_min_image_count(FRAMES_IN_FLIGHT)
    		.set_old_swapchain(Swapchain)
            .set_image_array_layer_count(1)
            .set_desired_extent(Extent.X, Extent.Y)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build()
            .value();

    	if(bFromResize)
    	{
    		vkDestroySwapchainKHR(Context->GetDevice()->GetDevice(), Swapchain, nullptr);
    	}

        Swapchain = vkbSwapchain.swapchain;
    	
        std::vector<VkImage> RawImages = vkbSwapchain.get_images().value();
    	
		SwapchainImages.clear();
    	
    	
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

			VK_CHECK(vkCreateImageView(Context->GetDevice()->GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));

        	FRHIImageDesc ImageDescription;
        	ImageDescription.Extent = Extent;
        	ImageDescription.Format = EImageFormat::R8_UNORM;
        	ImageDescription.NumMips = 1;
        	ImageDescription.NumSamples = 1;


        	TRefCountPtr<FVulkanImage> Image = MakeRefCount<FVulkanImage>(Context->GetDevice(), ImageDescription, RawImage, ImageView);
        	Image->SetInitialAccess(ERHIAccess::None);
        	Image->SetDefaultAccess(ERHIAccess::PresentRead);
        	
        	
			SwapchainImages.push_back(Image);
        }

    	size_t currentImageCount = RawImages.size();

    	// Destroy excess PresentSemaphores and resize down
    	if (PresentSemaphores.size() > currentImageCount)
    	{
    		for (size_t i = currentImageCount; i < PresentSemaphores.size(); i++)
    		{
    			vkDestroySemaphore(Context->GetDevice()->GetDevice(), PresentSemaphores[i], nullptr);
    		}

    		// Now safely resize down
    		PresentSemaphores.resize(currentImageCount);
    	}

    	// Ensure PresentSemaphores has enough slots (initialize with null)
    	if (PresentSemaphores.size() < currentImageCount)
    	{
		    for (auto Semaphore : PresentSemaphores)
		    {
		    	vkDestroySemaphore(Context->GetDevice()->GetDevice(), Semaphore, nullptr);
		    }
    		
    		PresentSemaphores.clear();
    		PresentSemaphores.resize(currentImageCount);
    	}

    	// Create new semaphores
    	for (size_t i = 0; i < currentImageCount; i++)
    	{
    		VkSemaphoreCreateInfo CreateInfo = {};
    		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    		vkDestroySemaphore(Context->GetDevice()->GetDevice(), PresentSemaphores[i], nullptr);
    		VK_CHECK(vkCreateSemaphore(Context->GetDevice()->GetDevice(),&CreateInfo,  nullptr, &PresentSemaphores[i]));
    		Context->SetVulkanObjectName("Present Semaphore: " + eastl::to_string(i), VK_OBJECT_TYPE_SEMAPHORE, (uint64)PresentSemaphores[i]);
    	}

    	// --------- AQUIRE SEMAPHORES MANAGEMENT --------- //
    	
    	// Destroy excess AquireSemaphores
    	if (AquireSemaphores.size() > FRAMES_IN_FLIGHT)
    	{
    		for (size_t i = FRAMES_IN_FLIGHT; i < AquireSemaphores.size(); i++)
    		{
    			vkDestroySemaphore(Context->GetDevice()->GetDevice(), AquireSemaphores[i], nullptr);
    		}
    		
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Ensure AquireSemaphores has enough slots
    	if (AquireSemaphores.size() < FRAMES_IN_FLIGHT)
    	{
    		for (auto Semaphore : AquireSemaphores)
    		{
    			vkDestroySemaphore(Context->GetDevice()->GetDevice(), Semaphore, nullptr);
    		}
    		
    		AquireSemaphores.clear();
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Create new Aquire semaphores
    	for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    	{
    		VkSemaphoreCreateInfo CreateInfo = {};
    		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    		vkDestroySemaphore(Context->GetDevice()->GetDevice(), AquireSemaphores[i], nullptr);
    		VK_CHECK(vkCreateSemaphore(Context->GetDevice()->GetDevice(),&CreateInfo,  nullptr, &AquireSemaphores[i]));
    		Context->SetVulkanObjectName("Aquire Semaphore: " + eastl::to_string(i), VK_OBJECT_TYPE_SEMAPHORE, (uint64)PresentSemaphores[i]);

    	}

    	
    	if(bFromResize)
    	{
    		return;
    	}
    	
        VkFenceCreateInfo FenceCreateInfo = {};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        Fences.reserve(FRAMES_IN_FLIGHT);
        for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            VkFence Fence;
            VK_CHECK(vkCreateFence(Context->GetDevice()->GetDevice(), &FenceCreateInfo, nullptr, &Fence));
        	Context->SetVulkanObjectName("Fence: " + eastl::to_string(i), VK_OBJECT_TYPE_FENCE, (uint64)Fence);

            Fences.push_back(Fence);
        }
    }

    void FVulkanSwapchain::RecreateSwapchain(const FIntVector2D& Extent)
    {
    	Context->WaitIdle();

    	CreateSwapchain(Context->GetVulkanInstance(), Context, Windowing::GetPrimaryWindowHandle(), Extent, true);

    	bNeedsResize = false;
    }

	void FVulkanSwapchain::SetPresentMode(VkPresentModeKHR NewMode)
    {
    	CurrentPresentMode = NewMode;
    	bNeedsResize = true;
    }

    TRefCountPtr<FVulkanImage> FVulkanSwapchain::GetCurrentImage() const
    {
	    return SwapchainImages[CurrentFrameIndex];
    }

    void FVulkanSwapchain::AquireNextImage(uint32 NewFrameIndex)
    {
    	CurrentFrameIndex = NewFrameIndex;

    	VkFenceCreateInfo FenceInfo = {};
    	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    	
    	VkResult Result = vkAcquireNextImageKHR(Context->GetDevice()->GetDevice(), Swapchain, VULKAN_TIMEOUT_ONE_SECOND, AquireSemaphores[CurrentFrameIndex], Fences[CurrentFrameIndex], &CurrentImageIndex);
    	
    	VK_CHECK(vkWaitForFences(Context->GetDevice()->GetDevice(), 1, &Fences[CurrentFrameIndex], VK_TRUE, VULKAN_TIMEOUT_ONE_SECOND));
    	VK_CHECK(vkResetFences(Context->GetDevice()->GetDevice(), 1, &Fences[CurrentFrameIndex]));
    	
    	if (Result == VK_SUBOPTIMAL_KHR || Result == VK_ERROR_OUT_OF_DATE_KHR)
    	{
    		bNeedsResize = true;
    	}
    	
    	VK_CHECK(Result);
    	
    }

    void FVulkanSwapchain::Present()
    {
    	VkPresentInfoKHR PresentInfo = {};
    	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    	PresentInfo.pSwapchains = &Swapchain;
    	PresentInfo.swapchainCount = 1;
    	PresentInfo.pWaitSemaphores = &PresentSemaphores[CurrentImageIndex];
    	PresentInfo.waitSemaphoreCount = 1;
    	PresentInfo.pImageIndices = &CurrentImageIndex;

    	VkResult Result = vkQueuePresentKHR(Context->GetQueue(ECommandQueue::Graphics)->Queue, &PresentInfo);

    	if (Result == VK_SUBOPTIMAL_KHR || Result == VK_ERROR_OUT_OF_DATE_KHR || bNeedsResize)
    	{
    		RecreateSwapchain(Windowing::GetPrimaryWindowHandle()->GetExtent());
    	}
	    else
	    {
    		VK_CHECK(Result);
	    }
    }
}

#endif