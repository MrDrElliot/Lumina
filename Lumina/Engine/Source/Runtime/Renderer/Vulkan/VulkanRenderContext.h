#pragma once
#include "VkBootstrap.h"
#include "..\RenderContext.h"
#include "vk_mem_alloc.h"
#include "VulkanDescriptor.h"
#include "VulkanTypes.h"


namespace Lumina
{
    class FVulkanSwapChain;
    
    class FVulkanRenderContext : public FRenderContext
    {
    public:

        FVulkanRenderContext();
        ~FVulkanRenderContext();

        /* Main Draw Loop */
        void Draw(float DeltaTime) override;
        void DrawBackground(VkCommandBuffer InBuffer);

        void FinalInitialization();

        /* Set Vulkan Devices, this is when logic is allowed to run, the swap chain has been created by this point */
        void InitializedDependencies(vkb::Device InDevice, vkb::PhysicalDevice InPhysicalDevice);

        void SetSwapChain(FVulkanSwapChain* InSwapChain) { ActiveSwapChain = InSwapChain; }
        void ClearSwapChain();
        

        VmaAllocator& GetAllocator() { return Allocator; }
        vkb::Instance GetInstance() const { return Instance; }
        FFrameData& GetCurrentFrame() { return Frames[FrameNumber % FRAME_OVERLAP]; }
        

    private:
        
        /* Main Initialization */
        void InternalInit() override;

        /* Initialize Command Buffers */
        void InitCommands();

        /* Initialize Sync Structures */
        void InitSyncStructures();
        
        /* Creates Vulkan instance */
        void CreateVkInstance();
        
        /* Initialize the allocator */
        void InitializeAllocator();

        /* Initialize Descriptors */
        void InitializeDescriptors();

        /* Initialize Pipelines */
        void InitPipelines();
        void InitBackgroundPipelines();
        

    private:
        
        vkb::Instance Instance;

        FFrameData Frames[FRAME_OVERLAP];
        uint64_t FrameNumber = 0;

        VkQueue GraphicsQueue;
        uint32_t GraphicsQueueFamily;

        FVulkanSwapChain* ActiveSwapChain;
        
        VkDebugUtilsMessengerEXT DebugMessenger;
        vkb::PhysicalDevice PhysicalDevice;
        vkb::Device Device;
        VmaAllocator Allocator;

        FDescriptorAllocator GlobalDescriptorAllocator;

        VkDescriptorSet DrawImageDescriptors;
        VkDescriptorSetLayout DrawImageDescriptorLayout;

        VkPipeline GradientPipeline;
        VkPipelineLayout GradientPipelineLayout;
        
        
        bool bInitialized = false;
    };
}
