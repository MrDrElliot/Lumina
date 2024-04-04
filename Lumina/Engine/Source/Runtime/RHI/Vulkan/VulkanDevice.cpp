#include "VulkanDevice.h"

#include "VulkanRendererContext.h"
#include "Source/Runtime/Log/Log.h"
#include <set>

namespace Lumina
{

    const std::vector DeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    LVulkanPhysicalDevice::LVulkanPhysicalDevice()
    {
        PhysicalDevice = VK_NULL_HANDLE;

        uint32_t DeviceCount = 0;
        vkEnumeratePhysicalDevices(LVulkanRendererContext::GetInstance(), &DeviceCount, nullptr);
        if(DeviceCount == 0)
        {
            LE_LOG_CRITICAL("Failed to find GPU with Vulkan support!");
            return;
        }

        std::vector<VkPhysicalDevice> Devices(DeviceCount);
        vkEnumeratePhysicalDevices(LVulkanRendererContext::GetInstance(), &DeviceCount, Devices.data());
        
        for(const auto& Device : Devices)
        {
            if(IsDeviceSuitable(Device))
            {
                LE_LOG_INFO("Found a suitable device!");
                PhysicalDevice = Device;
                break;
            }
        }

        if(PhysicalDevice == VK_NULL_HANDLE)
        {
            LE_LOG_CRITICAL("Failed to find a suitable GPU!");
            assert(0);
        }
        
    }

    LVulkanPhysicalDevice::~LVulkanPhysicalDevice()
    {
    }

    bool LVulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice InDevice) const
    {
        FQueueFamilyIndices Indicies = GetQueueFamilyIndices(InDevice);

        return Indicies.IsComplete();
    }

    bool LVulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice InDevice)
    {
        uint32_t ExtenionCount;
        vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &ExtenionCount, nullptr);

        std::vector<VkExtensionProperties> AvailableExtensions(ExtenionCount);
        vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &ExtenionCount, AvailableExtensions.data());

        std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

        for(const VkExtensionProperties& Extension : AvailableExtensions)
        {
            RequiredExtensions.erase(Extension.extensionName);
        }

        return RequiredExtensions.empty();
    }

    FQueueFamilyIndices LVulkanPhysicalDevice::GetQueueFamilyIndices(VkPhysicalDevice InDevice)
    {
        FQueueFamilyIndices Indicies;

        uint32_t QueueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, nullptr);
        if(QueueFamilyCount == 0)
        {
            LE_LOG_CRITICAL("Failed to find any queue families");
        }
        
        std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, QueueFamilies.data());

        int i = 0;
        for(const auto& QueueFamily : QueueFamilies)
        {
            if(QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                VkBool32 PresentSupport = false;
                Indicies.GraphicsFamily = i;
                Indicies.PresentFamily = i;

                if(Indicies.IsComplete()) break;
            }

            i++;
        }
        return Indicies;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
    LVulkanDevice::LVulkanDevice(LVulkanPhysicalDevice* InDevice, VkPhysicalDeviceFeatures InFeatures)
    {
        GraphicsQueue = nullptr;
        ComputeQueue = nullptr;
        PhysicalDevice = InDevice;
        EnabledFeatures = InFeatures;

        FQueueFamilyIndices QueueFamilyIndices = InDevice->GetQueueFamilyIndices(PhysicalDevice->GetPhysicalDevice());
        if(!QueueFamilyIndices.IsComplete())
        {
            LE_LOG_CRITICAL("Uncomplete queue family indicies!");
            assert(0);
        }
        
        std::set UniqueQueueFamilies = { QueueFamilyIndices.GraphicsFamily.value(), QueueFamilyIndices.PresentFamily.value() };

        std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
        
        
        float QueuePrio = 1.0f;
        for(uint32_t QFamily : UniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo QueueCreateInfo = {};
            QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            QueueCreateInfo.queueFamilyIndex = QFamily;
            QueueCreateInfo.queueCount = 1;
            QueueCreateInfo.pQueuePriorities = &QueuePrio;
            
            QueueCreateInfos.push_back(QueueCreateInfo);
        }

        
        VkPhysicalDeviceFeatures DevicesFeatures = {};


        VkDeviceCreateInfo DeviceCreateInfo = {};
        DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());
        DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
        DeviceCreateInfo.pEnabledFeatures = &DevicesFeatures;
        DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
        DeviceCreateInfo.enabledLayerCount = 0;
        DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());

        if (vkCreateDevice(PhysicalDevice->GetPhysicalDevice(), &DeviceCreateInfo, nullptr, &Device) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to create a logical device!");
            return;
        }

        vkGetDeviceQueue(Device, QueueFamilyIndices.GraphicsFamily.value(), 0, &GraphicsQueue);
        vkGetDeviceQueue(Device, QueueFamilyIndices.GraphicsFamily.value(), 0, &ComputeQueue);

    }

    LVulkanDevice::~LVulkanDevice()
    {
        Destroy();
    }

    void LVulkanDevice::Destroy()
    {
        vkDestroyDevice(Device, nullptr);
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
