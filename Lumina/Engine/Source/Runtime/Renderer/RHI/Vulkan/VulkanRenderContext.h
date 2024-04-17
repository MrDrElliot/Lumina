#pragma once

#include "Source/Runtime/Renderer/RenderContext.h"
#include <vulkan/vulkan_core.h>



namespace Lumina
{
    class FVulkanSwapchain;
    
    struct FQueueFamilyIndex 
    {
        glm::uint32 Graphics;
        glm::uint32 Transfer;
        glm::uint32 Compute;
        glm::uint32 Present;
    };
    
    class FVulkanRenderContext : public FRenderContext
    {
    public:

        FVulkanRenderContext(const FRenderConfig& InConfig);
        ~FVulkanRenderContext();

        static FVulkanRenderContext& Get() { return *Instance; }

        void Destroy() override;
        
        static VkInstance GetVulkanInstance() { return Instance->VulkanInstance; }
        static VkDevice GetDevice() { return Instance->Device; }
        static VkPhysicalDevice GetPhysicalDevice() { return Instance->PhysicalDevice; }
        static std::shared_ptr<FVulkanSwapchain> GetSwapchain() { return Instance->Swapchain; }
        static VkQueue GetGeneralQueue() { return Instance->GeneralQueue; }
        static FQueueFamilyIndex GetQueueFamilyIndex() { return Instance->QueueFamilyIndex; }

        static VkCommandBuffer AllocateTransientCommandBuffer();
        static void ExecuteTransientCommandBuffer(VkCommandBuffer CmdBuffer);

    private:

        static FVulkanRenderContext* Instance;

        VkInstance VulkanInstance;
        VkDebugUtilsMessengerEXT DebugMessenger;
        
        std::shared_ptr<FVulkanSwapchain> Swapchain;
        
        VkDevice Device;
        VkPhysicalDevice PhysicalDevice;

        VkCommandPool CommandPool;

        VkQueue GeneralQueue;
        FQueueFamilyIndex QueueFamilyIndex;
    };
}
