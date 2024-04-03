#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanDevice.h"

#include <vector>


namespace Lumina
{
	// Macro to get a procedure address based on a vulkan instance
	#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
	{                                                                       \
		fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint)); \
		HZ_CORE_ASSERT(fp##entrypoint);                                     \
	}
	
	// Macro to get a procedure address based on a vulkan device
	#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
	{                                                                       \
		fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
		HZ_CORE_ASSERT(fp##entrypoint);                                     \
	}
	
	static PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
	static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
	static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
	static PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	static PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	static PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	static PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	static PFN_vkQueuePresentKHR fpQueuePresentKHR;
	
	// Nvidia extensions
	inline PFN_vkCmdSetCheckpointNV fpCmdSetCheckpointNV;
	inline PFN_vkGetQueueCheckpointDataNV fpGetQueueCheckpointDataNV;
	
	VKAPI_ATTR inline void VKAPI_CALL vkCmdSetCheckpointNV(VkCommandBuffer commandBuffer, const void* pCheckpointMarker)
	{
		fpCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
	}
	
	VKAPI_ATTR inline void VKAPI_CALL vkGetQueueCheckpointDataNV(VkQueue queue, uint32_t* pCheckpointDataCount, VkCheckpointDataNV* pCheckpointData)
	{
		fpGetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
	}
	
	typedef struct FSwapChainBuffers
	{
		VkImage Iamge;
		VkImageView View;
	} SwapChainBuffer;
	
	class FVulkanSwapChain
	{
	public:

		FVulkanSwapChain(VkInstance InInstance, LVulkanDevice* InDevice, GLFWwindow* InWindow);

		void Create(uint32_t* InWidth, uint32_t* InHeight, bool VSync);
		void Destroy();


	public:

		VkFormat ColorFormat;
		VkColorSpaceKHR ColorSpace;
		VkSwapchainKHR SwapChain = VK_NULL_HANDLE;
		uint32_t ImageCount;
		std::vector<VkImage> Images;
		std::vector<SwapChainBuffer> Buffers;
		uint32_t QueueNodeIndex = UINT32_MAX;


	private:
		VkInstance Instance;
		LVulkanDevice* Device;
		VkPhysicalDevice PhysicalDevice;
		VkSurfaceKHR Surface;

	};
}