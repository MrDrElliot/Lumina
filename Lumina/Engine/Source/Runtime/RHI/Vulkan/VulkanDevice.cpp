#include "VulkanDevice.h"


namespace Lumina
{
    LVulkanPhysicalDevice::LVulkanPhysicalDevice()
    {
        
    }

    LVulkanPhysicalDevice::~LVulkanPhysicalDevice()
    {
    }

    LVulkanDevice::LVulkanDevice(LVulkanPhysicalDevice* InDevice, VkPhysicalDeviceFeatures InFeatures)
    {
        PhysicalDevice = InDevice;
        EnabledFeatures = InFeatures;

        const bool bEnableAftermath = true;

        std::vector<const char*> DeviceExtensions;

    }

    LVulkanDevice::~LVulkanDevice()
    {
    }

    void LVulkanDevice::Destroy()
    {
    }

    VkCommandBuffer LVulkanDevice::GetCommandBuffer(bool bBegin, bool bCompute)
    {
        return nullptr;
    }

    void LVulkanDevice::FlushCommandBuffer(VkCommandBuffer InCommandBuffer)
    {
    }

    void LVulkanDevice::FlushCommandBuffer(VkCommandBuffer InCommandBuffer, VkQueue InQueue)
    {
    }

    LVulkanCommandPool* LVulkanDevice::GetThreadLocalCommandPool()
    {
        return nullptr;
    }

    LVulkanCommandPool* LVulkanDevice::GetOrCreateThreadLocalCommandPool()
    {
        return nullptr;
    }
}
