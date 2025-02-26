
#include "Renderer/GPUBarrier.h"
#ifdef LUMINA_RENDERER_VULKAN

#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Core/Windows/Window.h"
#include "Renderer/RenderTypes.h"
#include "src/VkBootstrap.h"
#include "VulkanRenderContext.h"

#include "VulkanSwapchain.h"
#include <glfw/glfw3.h>

namespace Lumina
{
    FVulkanSwapchain::FVulkanSwapchain(VkInstance Instance, FVulkanRenderContext* Device, FWindow* Window, FVector2D Extent)
    {
        
    }

    FVulkanSwapchain::~FVulkanSwapchain()
    {
        
    }

    void FVulkanSwapchain::CreateSwapchain(VkInstance Instance, FVulkanRenderContext* Device, FWindow* Window, FVector2D Extent, bool bFromResize)
    {
        VK_CHECK(glfwCreateWindowSurface(Instance, Window->GetWindow(), nullptr, &Surface));
    	
        SwapchainImages.reserve(FRAMES_IN_FLIGHT);

    	bool bBeingResized = false;
        if(LIKELY(Swapchain))
        {
        	vkDeviceWaitIdle(Device->GetDevice());
            vkDestroySwapchainKHR(Device->GetDevice(), Swapchain, nullptr);
        	bBeingResized = true;
        }
    	
        vkb::SwapchainBuilder swapchainBuilder { Device->GetPhysicalDevice(), Device->GetDevice(), Surface };

    	Format = VK_FORMAT_B8G8R8A8_UNORM;
        SurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    	SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(SurfaceFormat)
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_min_image_count(FRAMES_IN_FLIGHT)
            .set_image_array_layer_count(1)
            .set_desired_extent(Extent.X, Extent.Y)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build()
            .value();

        Swapchain = vkbSwapchain.swapchain;
    	
        std::vector<VkImage> RawImages = vkbSwapchain.get_images().value();
    	
        SwapchainImages.clear();

    	FCommandList* CommandList = Device->BeginCommandList(ECommandQueue::Graphics, ECommandBufferUsage::Transient);
    	
        for (VkImage RawImage : RawImages)
        {
        	FRHIImageHandle ImageHandle = Device->CreateRenderTarget(Extent);
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
        	VulkanImage->Usage = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        	SwapchainImages.push_back(ImageHandle);

        	FGPUBarrier Barriers[] =
        	{
        		FGPUBarrier::Image(ImageHandle, EImageLayout::Undefined)
        	};
        	
        	Device->Barrier(Barriers, std::size(Barriers), CommandList);
        	
        }

    	Device->EndCommandList(CommandList, true);

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
    		
    		VK_CHECK(vkCreateSemaphore(Device->GetDevice(),&CreateInfo,  nullptr, &PresentSemaphores[i]));
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
    		AquireSemaphores.resize(FRAMES_IN_FLIGHT);
    	}

    	// Create new Aquire semaphores
    	for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    	{
    		VkSemaphoreCreateInfo CreateInfo = {};
    		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    		
    		VK_CHECK(vkCreateSemaphore(Device->GetDevice(),&CreateInfo,  nullptr, &AquireSemaphores[i]));
    	}

    	
    	if(bBeingResized)
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
            VK_CHECK(vkCreateFence(Device->GetDevice(), &FenceCreateInfo, nullptr, &Fence));
            Fences.push_back(Fence);
        }
    }

    FRHIImageHandle FVulkanSwapchain::GetCurrentImage() const
    {
	    return {};
    }
}

#endif