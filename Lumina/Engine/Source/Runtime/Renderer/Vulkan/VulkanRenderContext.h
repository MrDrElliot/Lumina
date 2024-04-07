#pragma once

#include <vk-bootstrap/src/VkBootstrap.h>
#include "..\RenderContext.h"
#include "VulkanDescriptor.h"
#include "VulkanLoader.h"
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
        void ImGuiDraw(float DeltaTime) override;
        void Draw(float DeltaTime) override;
        void DrawGeometry(VkCommandBuffer InCmd);
        void DrawBackground(VkCommandBuffer InBuffer);
        void DrawImGui(VkCommandBuffer InBuffer, VkImageView TargetViewImage);

        FAllocatedBuffer CreateBuffer(size_t Size, VkBufferUsageFlags InUsage, VmaMemoryUsage MemoryUsage);
        void DestroyBuffer(const FAllocatedBuffer& Buffer);

        FGPUMeshBuffers UploadMesh(std::span<uint32_t> Indices, std::span<FVertex> Vertices);

        /* Submits outside of the draw loop for an immediate render */
        void ImmediateSubmit(std::function<void(VkCommandBuffer Buffer)>&& Function);

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

        /* Initialize ImGui */
        void InitImGui();

        /* Initialize Pipelines */
        void InitPipelines();
        void InitBackgroundPipelines();
        void InitTrianglePipeline();
        void InitMeshPipeline();

        void InitDefaultData();

    private:
        
        vkb::Instance Instance;

        FFrameData Frames[FRAME_OVERLAP];
        uint64_t FrameNumber = 0;

        FGPUMeshBuffers Rect;


        std::vector<FComputeEffect> BackgroundEffects;
        int CurrentBackgroundEffect = 0;

        VkFence ImmediateFence;
        VkCommandBuffer ImmediateCommandBuffer;
        VkCommandPool ImmediateCommandPool;

        VkQueue GraphicsQueue;
        uint32_t GraphicsQueueFamily;

        FVulkanSwapChain* ActiveSwapChain;
        
        VkDebugUtilsMessengerEXT DebugMessenger;
        vkb::PhysicalDevice PhysicalDevice;
        vkb::Device Device;
        VmaAllocator Allocator;

        FDescriptorAllocator GlobalDescriptorAllocator;

        std::vector<std::shared_ptr<FMeshAsset>> testMeshes;


        VkDescriptorSet DrawImageDescriptors;
        VkDescriptorSetLayout DrawImageDescriptorLayout;

        VkPipeline GradientPipeline;
        VkPipelineLayout GradientPipelineLayout;

        VkPipeline TrianglePipeline;
        VkPipelineLayout TrianglePipelineLayout;

        VkPipeline MeshPipeline;
        VkPipelineLayout MeshPipelineLayout;
        
        
        bool bInitialized = false;
    };
}
