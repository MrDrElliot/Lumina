#pragma once

#define GLFW_INCLUDE_VULKAN
#include <optional>
#include <GLFW/glfw3.h>

#include "Source/Runtime/CoreObject/Object.h"
#include <map>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>


namespace Lumina
{
	struct FQueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};
	
//////////////////////////////////////////////////////////////////////////////////

	
	class LVulkanPhysicalDevice : public LObject
	{
	public:
	public:
		LVulkanPhysicalDevice();
		~LVulkanPhysicalDevice();
		
		bool IsDeviceSuitable(VkPhysicalDevice InDevice) const;
		
		VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }

		bool CheckDeviceExtensionSupport(VkPhysicalDevice InDevice);
		
		VkPhysicalDevice GetVulkanPhysicalDevice() const { return PhysicalDevice; }
		static FQueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice InDevice);

		const VkPhysicalDeviceProperties& GetProperties() const { return Properties; }
		const VkPhysicalDeviceLimits& GetLimits() const { return Properties.limits; }
		const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return MemoryProperties; }

		VkFormat GetDepthFormat() const { return m_DepthFormat; }


	private:

	
	private:

		FQueueFamilyIndices QueueFamilyIndices;

		VkPhysicalDevice PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties Properties;
		VkPhysicalDeviceFeatures Features;
		VkPhysicalDeviceMemoryProperties MemoryProperties;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
		std::unordered_set<std::string> SupportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;

		friend class VulkanDevice;
	};

//////////////////////////////////////////////////////////////////////////////////
	
	class LVulkanCommandPool : public LObject
	{
	public:
		LVulkanCommandPool();
		virtual ~LVulkanCommandPool();

		
		VkCommandBuffer AllocateCommandBuffer(bool begin, bool compute = false);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);

		VkCommandPool GetGraphicsCommandPool() const { return GraphicsCommandPool; }
		VkCommandPool GetComputeCommandPool() const { return ComputeCommandPool; }
	private:
		VkCommandPool GraphicsCommandPool;
		VkCommandPool ComputeCommandPool;
	};

	//////////////////////////////////////////////////////////////////////////////////

	
	class LVulkanDevice : public LObject
	{
	public:

		LVulkanDevice(LVulkanPhysicalDevice* InDevice, VkPhysicalDeviceFeatures InFeatures);
		virtual ~LVulkanDevice();

		void Destroy();
		
		VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
		VkQueue GetComputeQueue() const { return ComputeQueue; }

		VkCommandBuffer GetCommandBuffer(bool bBegin, bool bCompute = false);
		void FlushCommandBuffer(VkCommandBuffer InCommandBuffer);
		void FlushCommandBuffer(VkCommandBuffer InCommandBuffer, VkQueue InQueue);
		
		LVulkanPhysicalDevice* GetPhysicalDevice() const { return PhysicalDevice; }
		VkDevice GetVulkanDevice() const { return Device; }

		LVulkanCommandPool* GetThreadLocalCommandPool();
		LVulkanCommandPool* GetOrCreateThreadLocalCommandPool();

	private:

		VkDevice Device = nullptr;
		LVulkanPhysicalDevice* PhysicalDevice = nullptr;
		VkPhysicalDeviceFeatures EnabledFeatures;

		VkQueue GraphicsQueue;
		VkQueue ComputeQueue;

		std::map<std::thread::id, LVulkanCommandPool*> CommandPool;
	};
}