#pragma once

#include <unordered_set>

#include "Renderer/RenderContext.h"
#include <vulkan/vulkan_core.h>

#include "Containers/String.h"

struct VulkanExtensionSet
{
    std::unordered_set<LString> instance;
    std::unordered_set<LString> layers;
    std::unordered_set<LString> device;
};

namespace Lumina
{
    class FImageSampler;
    class FVulkanSwapchain;
    
    
    class FVulkanRenderContext : public FRenderContext
    {
    public:

        FVulkanRenderContext(const FRenderConfig& InConfig);
        FVulkanRenderContext(const FVulkanRenderContext&) = delete;
        ~FVulkanRenderContext() override;


        static FVulkanRenderContext& Get() { return *static_cast<FVulkanRenderContext*>(Instance); }
        void Destroy() override;
        
        static VkInstance GetVulkanInstance() { return Get().VulkanInstance; }
        static VkDevice GetDevice() { return Get().Device; }
        static VkPhysicalDevice GetPhysicalDevice() { return Get().PhysicalDevice; }
        static void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties& OutProps) { OutProps = Get().PhysicalDeviceProperties; }
        static TRefPtr<FVulkanSwapchain>& GetSwapchain() { return Get().Swapchain; }
        static VkQueue GetGeneralQueue() { return Get().GeneralQueue; }

        static VkCommandBuffer AllocateTransientCommandBuffer();
        static void ExecuteTransientCommandBuffer(VkCommandBuffer CmdBuffer);

    private:
        
        VkInstance VulkanInstance;
        VkDebugUtilsMessengerEXT DebugMessenger;
        
        TRefPtr<FVulkanSwapchain> Swapchain;
        
        VkDevice Device;
        VkPhysicalDevice PhysicalDevice;
        VkPhysicalDeviceProperties PhysicalDeviceProperties;

        VkCommandPool CommandPool;

        VkQueue GeneralQueue;
        
    };
}
