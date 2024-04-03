#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanDevice.h"

namespace Lumina
{
	class FVulkanSwapChain
	{
	public:

		FVulkanSwapChain() {}
		FVulkanSwapChain(VkInstance* InInstance, const LVulkanDevice* InDevice, GLFWwindow* InWindow);

		void Create(uint32_t* InWidth, uint32_t* InHeight, bool VSync);
		void Destroy();

	};
}