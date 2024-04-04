#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include <vector>

#include "VulkanDevice.h"


namespace Lumina
{
	class FVulkanPipeline;
	class FVulkanRenderPass;

	typedef struct FSwapChainBuffers
	{
		VkImage Iamge;
		VkImageView View;
	} SwapChainBuffer;

	struct FSwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};
	
	class FVulkanSwapChain
	{
	public:

		FVulkanSwapChain(VkInstance InInstance, LVulkanDevice* InDevice, GLFWwindow* InWindow);

		void Create(uint32_t* InWidth, uint32_t* InHeight, bool VSync);
		void Destroy();

		void InitSurface(GLFWwindow* InWindow);

		void Present();

		VkExtent2D GetExtent() const { return Extent; }

		FSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice InDevice);

		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes);

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& InCapabilities) const;

		void RecordCommandBuffer(VkCommandBuffer InCommandBuffer, uint32_t ImageIndex);

		void CreateImageViews();

	public:
		
		VkSemaphore ImageAvailableSemaphore;
		VkSemaphore RenderFinishedSemaphore;
		VkFence InFlightFence;

		FVulkanRenderPass* RenderPass;
		FVulkanPipeline* SwapPipeline;
		
		VkFormat ColorFormat;
		VkColorSpaceKHR ColorSpace;
		VkSwapchainKHR SwapChain = VK_NULL_HANDLE;
		uint32_t ImageCount;
		VkExtent2D Extent;
		VkFormat ImageFormat;
		std::vector<VkImage> Images;
		std::vector<VkImageView> ImageViews;
		std::vector<SwapChainBuffer> Buffers;
		uint32_t QueueNodeIndex = UINT32_MAX;
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;

		std::vector<VkFramebuffer> FrameBuffers;


	private:
		VkInstance Instance;
		GLFWwindow* Window;
		LVulkanDevice* Device;
		VkPhysicalDevice PhysicalDevice;
		VkSurfaceKHR Surface;

	};
}
