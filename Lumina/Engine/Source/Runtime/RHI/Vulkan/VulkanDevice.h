#pragma once

#include "Source/Runtime/CoreObject/Object.h"
#include <map>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Lumina
{


	class LVulkanPhysicalDevice : public LObject
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};
	public:
		LVulkanPhysicalDevice();
		~LVulkanPhysicalDevice();

		bool IsExtensionSupported(const std::string& extensionName) const;
		uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		const VkPhysicalDeviceProperties& GetProperties() const { return m_Properties; }
		const VkPhysicalDeviceLimits& GetLimits() const { return m_Properties.limits; }
		const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return m_MemoryProperties; }

		VkFormat GetDepthFormat() const { return m_DepthFormat; }

		static LVulkanPhysicalDevice* Select();
	private:
		VkFormat FindDepthFormat() const;
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);
	private:
		QueueFamilyIndices m_QueueFamilyIndices;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		friend class VulkanDevice;
	};

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