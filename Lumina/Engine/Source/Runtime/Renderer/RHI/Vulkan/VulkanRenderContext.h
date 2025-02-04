#pragma once
#include "Renderer/RenderContext.h"
#include <vulkan/vulkan_core.h>


namespace Lumina
{
    class FVulkanCommandBuffer;

    class FImageSampler;
    class FVulkanSwapchain;

    struct FVulkanRenderContextFunctions
    {
        VkDebugUtilsMessengerEXT DebugMessenger;
        PFN_vkSetDebugUtilsObjectNameEXT DebugUtilsObjectNameEXT;
    };
    
    class FVulkanRenderContext : public IRenderContext
    {
    public:
        FVulkanRenderContext(const FRenderConfig& InConfig)
            : IRenderContext(InConfig),
              DescriptorPool(VK_NULL_HANDLE),
              VulkanInstance(VK_NULL_HANDLE),
              VulkanRenderContextFunctions(),
              Device(VK_NULL_HANDLE),
              PhysicalDevice(VK_NULL_HANDLE),
              PhysicalDeviceProperties(),
              CommandPool(VK_NULL_HANDLE),
              GeneralQueue(VK_NULL_HANDLE)
        {}

        FVulkanRenderContext(const FVulkanRenderContext&) = delete;
        ~FVulkanRenderContext() override;

        void Initialize() override;
        VkInstance GetVulkanInstance() { return VulkanInstance; }
        VkDevice GetDevice() { return Device; }
        VkPhysicalDevice GetPhysicalDevice() { return PhysicalDevice; }
        void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties& OutProps) { OutProps = PhysicalDeviceProperties; }
        VkQueue GetGeneralQueue() { return GeneralQueue; }
        VkCommandPool GetCommandPool() { return CommandPool; }

        FVulkanRenderContextFunctions GetRenderContextFunctions() { return VulkanRenderContextFunctions; }

        TVector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout InLayout, uint32 InCount);
        void FreeDescriptorSets(const TVector<VkDescriptorSet>& InSets);
        
        
        VkCommandBuffer AllocateTransientCommandBuffer();
        void ExecuteTransientCommandBuffer(VkCommandBuffer CmdBuffer);

    private:
        
        VkDescriptorPool                        DescriptorPool;
        VkInstance                              VulkanInstance;
        FVulkanRenderContextFunctions           VulkanRenderContextFunctions;
        VkDevice                                Device;
        VkPhysicalDevice                        PhysicalDevice;
        VkPhysicalDeviceProperties              PhysicalDeviceProperties;
        VkCommandPool                           CommandPool;
        VkQueue                                 GeneralQueue;
        
    };
}

