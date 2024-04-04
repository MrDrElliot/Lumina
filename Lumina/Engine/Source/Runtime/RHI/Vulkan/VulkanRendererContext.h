#pragma once

#include "Source/Runtime/RHI/RendererContext.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "VulkanDevice.h"

namespace Lumina
{
    class LVulkanRendererContext : public LRendererContext
    {
    public:

        
        
        LVulkanRendererContext();
        ~LVulkanRendererContext();
        
        void Init() override;
        void OnDestroy() override;

        static VkInstance GetInstance() { return Instance; }

        LVulkanPhysicalDevice* GetPhysicalDevice() const { return PhysicalDevice; }
        LVulkanDevice* GetDevice() const { return Device; }
        
        bool CheckValidationLayerSupport();


    private:

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        
       static VkInstance Instance;

       LVulkanPhysicalDevice* PhysicalDevice;
       LVulkanDevice* Device;
       VkDebugUtilsMessengerEXT debugMessenger;

    };
}
