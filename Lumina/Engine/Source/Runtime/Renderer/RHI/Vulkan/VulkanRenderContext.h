#pragma once
#include "Renderer/RenderContext.h"
#include <vulkan/vulkan_core.h>


namespace Lumina
{
    class FVulkanMemoryAllocator;
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
        FVulkanRenderContext()
            : IRenderContext()
            , DescriptorPool(VK_NULL_HANDLE),
              VulkanInstance(VK_NULL_HANDLE),
              VulkanRenderContextFunctions(),
              Device(VK_NULL_HANDLE),
              PhysicalDevice(VK_NULL_HANDLE)
              ,PhysicalDeviceFeatures(),
              PhysicalDeviceProperties()
              ,PhysicslDeviceMemoryProperties(),
              CommandPool(VK_NULL_HANDLE),
              GeneralQueue(VK_NULL_HANDLE)
        {
        }

        FVulkanRenderContext(const FVulkanRenderContext&) = delete;
        ~FVulkanRenderContext() override;

        void Initialize() override;

        FRHIShader CreateShader(const FString& ShaderPath) override;
        FRHIBuffer CreateBuffer(const FDeviceBufferSpecification& Spec, void* Data = nullptr, uint64 DataSize = 0) override;
        FRHIImage CreateImage(const FImageSpecification& ImageSpec) override;

        
        FORCEINLINE VkInstance GetVulkanInstance() { return VulkanInstance; }
        FORCEINLINE VkDevice GetDevice() { return Device; }
        FORCEINLINE VkPhysicalDevice GetPhysicalDevice() { return PhysicalDevice; }
        
        FORCEINLINE const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return PhysicalDeviceProperties; }
        FORCEINLINE const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const { return PhysicalDeviceFeatures; }
        FORCEINLINE const VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() const { return PhysicslDeviceMemoryProperties; }
        
        FORCEINLINE VkQueue GetGeneralQueue() { return GeneralQueue; }
        FORCEINLINE VkCommandPool GetCommandPool() { return CommandPool; }

        FORCEINLINE FVulkanRenderContextFunctions GetRenderContextFunctions() { return VulkanRenderContextFunctions; }
        
        FORCEINLINE VkDescriptorPool GetVkDescriptorPool() const { return DescriptorPool; }

        TVector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout InLayout, uint32 InCount);
        void FreeDescriptorSets(const TVector<VkDescriptorSet>& InSets);

        
        VkCommandBuffer AllocateTransientCommandBuffer();
        void ExecuteTransientCommandBuffer(VkCommandBuffer CmdBuffer);

    private:

        VkInstance                              VulkanInstance;
        VkPhysicalDevice                        PhysicalDevice;
        VkDevice                                Device;
        VkDescriptorPool                        DescriptorPool;
        FVulkanRenderContextFunctions           VulkanRenderContextFunctions;
        VkPhysicalDeviceFeatures                PhysicalDeviceFeatures;
        VkPhysicalDeviceProperties              PhysicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties        PhysicslDeviceMemoryProperties;
        VkCommandPool                           CommandPool;
        
        VkQueue                                 GeneralQueue;
        VkQueue                                 TransferQueue;
        VkQueue                                 PresentQueue;
        VkQueue                                 GraphicsQueue;
        VkQueue                                 ComputeQueue;
        
    };
}

