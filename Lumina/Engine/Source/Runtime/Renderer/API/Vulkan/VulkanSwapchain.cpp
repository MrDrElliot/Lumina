
#include "Renderer/GPUBarrier.h"
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
	    for (FRHIImageHandle& Image : SwapchainImages)
	    {
	    	FVulkanImage* VulkanImage = Context->GetImagePool().GetResource(Image);
		    vkDestroyImageView(Context->GetDevice(), VulkanImage->ImageView, nullptr);
	    }

    	vkDestroySwapchainKHR(Context->GetDevice(), Swapchain, nullptr);

    	for (VkFence Fence : Fences)
    	{
    		vkDestroyFence(Context->GetDevice(), Fence, nullptr);
    	}

    	for (VkSemaphore Semaphore : AquireSemaphores)
    	{
    		vkDestroySemaphore(Context->GetDevice(), Semaphore, nullptr);    
    	}

    	for (VkSemaphore Semaphore : PresentSemaphores)
    	{
    		vkDestroySemaphore(Context->GetDevice(), Semaphore, nullptr);    
    	}

    	
    	vkDestroySurfaceKHR(Context->GetVulkanInstance(), Surface, nullptr);
    	
    	Fences.clear();
    	AquireSemaphores.clear();
    	PresentSemaphores.clear();
    	SwapchainImages.clear();
    }

    void FVulkanSwapchain::CreateSwapchain(VkInstance Instance, FVulkanRenderContext* Device, FWindow* Window, FIntVector2D Extent, bool bFromResize)
    {
    	Context = Device;
    	SwapchainExtent = Extent;

    	if (bFromResize == false)
    	{
			VK_CHECK(glfwCreateWindowSurface(Instance, Window->GetWindow(), nullptr, &Surface));
    	}
    	
        SwapchainImages.reserve(FRAMES_IN_FLIGHT);

        if(bFromResize)
        {
            vkDestroySwapchainKHR(Device->GetDevice(), Swapchain, nullptr);
        }
    	
        vkb::SwapchainBuilder swapchainBuilder { Device->GetPhysicalDevice(), Device->GetDevice(), Surface };

    	Format = VK_FORMAT_B8G8R8A8_UNORM;
        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    	SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(SurfaceFormat)
            .set_desired_present_mode(CurrentPresentMode)
            .set_desired_min_image_count(FRAMES_IN_FLIGHT)
            .set_image_array_layer_count(1)
            .set_desired_extent(Extent.X, Extent.Y)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build()
            .value();

        Swapchain = vkbSwapchain.swapchain;
    	
        std::vector<VkImage> RawImages = vkbSwapchain.get_images().value();
    	
        for (auto SwapchainImage : SwapchainImages)
        {
			Device->GetImagePool().Free(SwapchainImage);    
        }
    	
        SwapchainImages.clear();

    	FCommandList* CommandList = Device->BeginCommandList(ECommandBufferLevel::Primary, ECommandQueue::Graphics, ECommandBufferUsage::Transient);
    	
        for (VkImage RawImage : RawImages)
        {
        	FRHIImageHandle ImageHandle = Device->AllocateImage();
        	FVulkanImage* VulkanImage = Device->GetImagePool().GetResource(ImageHandle);
        	
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
        	
			VK_CHECK(vkCreateImageView(Device->GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));
			VulkanImage->ImageView = ImageView;
        	VulkanImage->Image = RawImage;
        	VulkanImage->DefaultLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        	SwapchainImages.push_back(ImageHandle);

        	FGPUBarrier Barriers[] =
        	{
        		FGPUBarrier::Image(ImageHandle, EImageLayout::Undefined)
        	};
        	
        	Device->Barrier(Barriers, std::size(Barriers), CommandList);
        	
        }

    	Device->EndCommandList(CommandList);

    	size_t currentImageCount = RawImages.size();

    	// Destroy excess PresentSemaphores and resize down
    	if (PresentSemaphores.size() > currentImageCount)
    	{
    		for (size_t i = currentImageCount; i < PresentSemaphores.size(); i++)
    		{
    			vkDestroySemaphore(Device->GetDevice(), PresentSemaphores[i], nullptr);
    		}

    		// Now safely resize down
    		PresentSemaphores.resize(currentImageCount);
    	}

    	// Ensure PresentSemaphores has enough slots (initialize with null)
    	if (PresentSemaphores.size() < currentImageCount)
    	{
		    for (auto Semaphore : PresentSemaphores)
		    {
		    	vkDestroySemaphore(Device->GetDevice(), Semaphore, nullptr);
		    }
    		
    		PresentSemaphores.clear();
    		PresentSemaphores.resize(currentImageCount);
    	}

    	// Create new semaphores
    	for (size_t i = 0; i < currentImageCount; i++)
    	{
    		VkSemaphoreCreateInfo CreateInfo = {};
    		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    		vkDestroySemaphore(Device->GetDevice(), PresentSemaphores[i], nullptr);
    		VK_CHECK(vkCreateSemaphore(Device->GetDevice(),&CreateInfo,  nullptr, &PresentSemaphores[i]));
    		Context->SetVulkanObjectName("Present Semaphore: " + eastl::to_string(i), VK_OBJECT_TYPE_SEMAPHORE, (uint64)PresentSemaphores[i]);
    	}

    	// --------- AQUIRE SEMAPHORES MANAGEMENT --------- //
    	
    	// Destroy excess AquireSemaphores
    	if (AquireSemaphores.size() > FRAMES_IN_FLIGHT)
    	{
    		for (size_t i = FRAMES_IN_FLIGHT; i < AquireSemaphores.size(); i++)
    		{
    			vkDestroySemaphore(Device->GetDevice(), AquireSemaphores[i], nullptr);
    		}
    		
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Ensure AquireSemaphores has enough slots
    	if (AquireSemaphores.size() < FRAMES_IN_FLIGHT)
    	{
    		for (auto Semaphore : AquireSemaphores)
    		{
    			vkDestroySemaphore(Device->GetDevice(), Semaphore, nullptr);
    		}
    		
    		AquireSemaphores.clear();
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Create new Aquire semaphores
    	for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    	{
    		VkSemaphoreCreateInfo CreateInfo = {};
    		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    		vkDestroySemaphore(Device->GetDevice(), AquireSemaphores[i], nullptr);
    		VK_CHECK(vkCreateSemaphore(Device->GetDevice(),&CreateInfo,  nullptr, &AquireSemaphores[i]));
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
            VK_CHECK(vkCreateFence(Device->GetDevice(), &FenceCreateInfo, nullptr, &Fence));
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

    FRHIImageHandle FVulkanSwapchain::GetCurrentImage() const
    {
	    return SwapchainImages[CurrentFrameIndex];
    }

    void FVulkanSwapchain::AquireNextImage(uint32 NewFrameIndex)
    {
    	CurrentFrameIndex = NewFrameIndex;

    	VkFenceCreateInfo FenceInfo = {};
    	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    	
    	VkResult Result = vkAcquireNextImageKHR(Context->GetDevice(), Swapchain, VULKAN_TIMEOUT_ONE_SECOND, AquireSemaphores[CurrentFrameIndex], Fences[CurrentFrameIndex], &CurrentImageIndex);
    	
    	VK_CHECK(vkWaitForFences(Context->GetDevice(), 1, &Fences[CurrentFrameIndex], VK_TRUE, VULKAN_TIMEOUT_ONE_SECOND));
    	VK_CHECK(vkResetFences(Context->GetDevice(), 1, &Fences[CurrentFrameIndex]));
    	
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

    	VkResult Result = vkQueuePresentKHR(Context->GetCommandQueues().GraphicsQueue, &PresentInfo);

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