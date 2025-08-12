
#include "VulkanBarriers.h"
#include "VulkanCommandList.h"
#include "VulkanDevice.h"
#include "Renderer/CommandList.h"
#include "..\..\StateTracking.h"
#include "Core/Profiler/Profile.h"
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
    	vkDestroySwapchainKHR(Context->GetDevice()->GetDevice(), Swapchain, VK_ALLOC_CALLBACK);

    	SwapchainImages.clear();

    	for (VkSemaphore Semaphore : AcquireSemaphores)
    	{
    		if (Semaphore)
    		{
    			vkDestroySemaphore(Context->GetDevice()->GetDevice(), Semaphore, VK_ALLOC_CALLBACK);
    		}
    	}
    	
    	for (VkSemaphore Semaphore : PresentSemaphores)
    	{
    		if (Semaphore)
    		{
    			vkDestroySemaphore(Context->GetDevice()->GetDevice(), Semaphore, VK_ALLOC_CALLBACK);    
    		}
    	}

    	AcquireSemaphores.clear();
    	PresentSemaphores.clear();

    	
    	vkDestroySurfaceKHR(Context->GetVulkanInstance(), Surface, VK_ALLOC_CALLBACK);
    	
    }

    void FVulkanSwapchain::CreateSwapchain(VkInstance Instance, FVulkanRenderContext* InContext, FWindow* Window, FIntVector2D Extent, bool bFromResize)
    {
    	LUMINA_PROFILE_SCOPE();

    	Context = InContext;
    	SwapchainExtent = Extent;
    	VkDevice Device = Context->GetDevice()->GetDevice();
    	

    	if (bFromResize == false)
    	{
			VK_CHECK(glfwCreateWindowSurface(Instance, Window->GetWindow(), VK_ALLOC_CALLBACK, &Surface));
    	}
	    
    	
		SwapchainImages.clear();
        SwapchainImages.reserve(SWAPCHAIN_IMAGES);
    	
        vkb::SwapchainBuilder swapchainBuilder { Context->GetDevice()->GetPhysicalDevice(), Device, Surface };

    	Format = VK_FORMAT_B8G8R8A8_UNORM;
        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    	SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(SurfaceFormat)
            .set_desired_present_mode(CurrentPresentMode)
            .set_desired_min_image_count(SWAPCHAIN_IMAGES)
    		.set_old_swapchain(Swapchain)
            .set_image_array_layer_count(1)
    		.set_allocation_callbacks(VK_ALLOC_CALLBACK)
            .set_desired_extent(Extent.X, Extent.Y)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build()
            .value();

    	if (bFromResize)
    	{
			vkDestroySwapchainKHR(Device, Swapchain, VK_ALLOC_CALLBACK);
    		Swapchain = VK_NULL_HANDLE;
		}

        Swapchain = vkbSwapchain.swapchain;
    	
        std::vector<VkImage> RawImages = vkbSwapchain.get_images().value();
    	

    	ICommandList* CommandList = Context->GetCommandList(Q_Graphics);
    	CommandList->Open();
    	
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

			VK_CHECK(vkCreateImageView(Device, &ImageViewCreateInfo, VK_ALLOC_CALLBACK, &ImageView));

        	FRHIImageDesc ImageDescription;
        	ImageDescription.Extent = Extent;
        	ImageDescription.Format = EFormat::R8_UNORM;
        	ImageDescription.NumMips = 1;
        	ImageDescription.NumSamples = 1;
        	ImageDescription.InitialState = EResourceStates::Present;
        	ImageDescription.bKeepInitialState = true;
        	ImageDescription.DebugName = "Swapchain Image";

        	TRefCountPtr<FVulkanImage> Image = MakeRefCount<FVulkanImage>(Context->GetDevice(), ImageDescription, RawImage, ImageView);
        	
			SwapchainImages.push_back(Image);
        }

    	CommandList->Close();
    	Context->ExecuteCommandList(CommandList, Q_Graphics);

    	
    	SIZE_T NumPresentSemaphores = SwapchainImages.size();
    	PresentSemaphores.reserve(NumPresentSemaphores);
    	for (SIZE_T i = 0; i < NumPresentSemaphores; ++i)
    	{
    		VkSemaphoreCreateInfo CreateInfo = {};
    		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    		VkSemaphore Semaphore;
    		VK_CHECK(vkCreateSemaphore(Device, &CreateInfo, VK_ALLOC_CALLBACK, &Semaphore));
    		PresentSemaphores.push_back(Semaphore);
    	}

    	SIZE_T NumAcquireSemaphores = std::max<uint32>(FRAMES_IN_FLIGHT, SwapchainImages.size());
		AcquireSemaphores.reserve(NumAcquireSemaphores);

	    for (SIZE_T i = 0; i < NumAcquireSemaphores; ++i)
	    {
	    	VkSemaphoreCreateInfo CreateInfo = {};
	    	CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	    	VkSemaphore Semaphore;
	    	VK_CHECK(vkCreateSemaphore(Device, &CreateInfo, VK_ALLOC_CALLBACK, &Semaphore));
	    	AcquireSemaphores.push_back(Semaphore);
	    }
    	
    }

    void FVulkanSwapchain::RecreateSwapchain(const FIntVector2D& Extent)
    {
    	LUMINA_PROFILE_SCOPE();

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
	    return SwapchainImages[CurrentImageIndex];
    }

    bool FVulkanSwapchain::AcquireNextImage()
    {
    	LUMINA_PROFILE_SCOPE();

    	VkSemaphore Semaphore = AcquireSemaphores[AcquireSemaphoreIndex];
    	VkResult Result = VK_RESULT_MAX_ENUM;
    	
	   	Result = vkAcquireNextImageKHR(Context->GetDevice()->GetDevice(), Swapchain, UINT64_MAX, Semaphore, nullptr, &CurrentImageIndex);
    	if (Result == VK_SUBOPTIMAL_KHR || Result == VK_ERROR_OUT_OF_DATE_KHR)
    	{
    		bNeedsResize = true;
    	}
    	
    	AcquireSemaphoreIndex = (AcquireSemaphoreIndex + 1) % AcquireSemaphores.size();
    	
    	if (Result == VK_SUCCESS || Result == VK_SUBOPTIMAL_KHR)
    	{
    		Context->GetQueue(ECommandQueue::Graphics)->AddWaitSemaphore(Semaphore, 0);
    		return true;
    	}

    	return false;
    }

    bool FVulkanSwapchain::Present()
    {
	    LUMINA_PROFILE_SCOPE();

    	VkSemaphore Semaphore = PresentSemaphores[CurrentImageIndex];

    	FQueue* Queue = Context->GetQueue(ECommandQueue::Graphics);
    	
    	Queue->AddSignalSemaphore(Semaphore, 0);
    	
    	Context->ExecuteCommandLists(nullptr, 0, ECommandQueue::Graphics);
    	
    	VkPresentInfoKHR PresentInfo = {};
    	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    	PresentInfo.pSwapchains = &Swapchain;
    	PresentInfo.swapchainCount = 1;
    	PresentInfo.pWaitSemaphores = &Semaphore;
    	PresentInfo.waitSemaphoreCount = 1;
    	PresentInfo.pImageIndices = &CurrentImageIndex;

    	VkResult Result;
	    {
	    	LUMINA_PROFILE_SECTION("vkQueuePresentKHR");
		    Result = vkQueuePresentKHR(Queue->Queue, &PresentInfo);
	    }

    	if (!(Result == VK_SUCCESS || Result == VK_SUBOPTIMAL_KHR || Result == VK_ERROR_OUT_OF_DATE_KHR) || bNeedsResize)
    	{
    		RecreateSwapchain(Windowing::GetPrimaryWindowHandle()->GetExtent());
    	}

#ifndef _WIN32

    	if (CurrentPresentMode == VK_PRESENT_MODE_FIFO_KHR)
    	{
    		Queue->WaitIdle();
    	}
#endif

#if 1
    	while (!FramesInFlight.empty())
#else
    	while (FramesInFlight.size() >= FRAMES_IN_FLIGHT)
#endif
    	{
    		FRHIEventQueryRef Query = FramesInFlight.front();
    		FramesInFlight.pop();
			
    		Context->WaitEventQuery(Query);

    		QueryPool.push_back(Query);
    	}

    	FRHIEventQueryRef Query;
    	if (!QueryPool.empty())
    	{
    		Query = QueryPool.back();
    		QueryPool.pop_back();
    	}
	    else
	    {
		    Query = Context->CreateEventQuery();
	    }

    	Context->ResetEventQuery(Query);
    	Context->SetEventQuery(Query, ECommandQueue::Graphics);
    	FramesInFlight.push(Query);
    	return true;
    }
}

#endif