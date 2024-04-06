#pragma once
#include "VkBootstrap.h"
#include "..\RenderContext.h"


#include "vk_mem_alloc.h"
#include "VulkanTypes.h"


namespace Lumina
{
    class FVulkanRenderContext : public FRenderContext
    {
    public:

        FVulkanRenderContext();
        ~FVulkanRenderContext();



        /* Main Draw Loop */
        void Draw(float DeltaTime) override;

        /* Set Vulkan Devices, this is when logic is allowed to run, the swap chain has been created by this point */
        void InitializedDependencies(vkb::Device InDevice, vkb::PhysicalDevice InPhysicalDevice);

        void SetSwapChain(VkSwapchainKHR InSwapChain) { ActiveSwapChain = InSwapChain; }
        void ClearSwapChain();
        

        vkb::Instance GetInstance() const { return VkbInstance; }
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

    private:
        
        vkb::Instance VkbInstance;

        FFrameData Frames[FRAME_OVERLAP];
        uint64_t FrameNumber = 0;

        VkQueue GraphicsQueue;
        uint32_t GraphicsQueueFamily;

        VkSwapchainKHR ActiveSwapChain;
        
        VkDebugUtilsMessengerEXT DebugMessenger;
        vkb::PhysicalDevice PhysicalDevice;
        vkb::Device Device;
        VmaAllocator Allocator;

        

        bool bInitialized = false;
    };
}
