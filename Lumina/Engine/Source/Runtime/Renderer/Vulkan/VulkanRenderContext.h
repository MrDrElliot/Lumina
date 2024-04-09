#pragma once

#include <vk-bootstrap/src/VkBootstrap.h>
#include "..\RenderContext.h"
#include "VulkanDescriptor.h"
#include "VulkanLoader.h"
#include "VulkanMaterial.h"
#include "VulkanSwapChain.h"
#include "VulkanTypes.h"

#include "Source/Runtime/Events/Delegate.h"


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
        void DrawGeometry(VkCommandBuffer InCmd);
        void DrawBackground(VkCommandBuffer InBuffer);
        void DrawImGui(VkCommandBuffer InBuffer, VkImageView TargetViewImage);
        
        bool BeginFrame(VkCommandBuffer& OutCmdInCmd, uint32_t* InSwapChainImageIndex);
        void SubmitFrame(VkCommandBuffer InCmd, uint32_t SwapChainImageIndex);

        FAllocatedImage CreateImage(VkExtent3D InSize, VkFormat InFormat, VkImageUsageFlags InUsage, bool bMipmapped = false);
        FAllocatedImage CreateImage(void* InData, VkExtent3D InSize, VkFormat InFormat, VkImageUsageFlags InUsage, bool bMipmapped = false);

        
        void DestroyImage(const FAllocatedImage& InImage);

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
        

        VkQueue GetGraphicsQueue() { return GraphicsQueue; }
        FVulkanSwapChain* GetActiveSwapChain() { return ActiveSwapChain; }
        VkDescriptorSetLayout GetGPUDescriptorLayout() { return GpuSceneDataDescriptorLayout; }
        vkb::Device GetDevice() { return Device; }
        VmaAllocator& GetAllocator() { return Allocator; }
        vkb::Instance GetInstance() const { return Instance; }
        FFrameData& GetCurrentFrame() { return Frames[FrameNumber % FRAME_OVERLAP]; }
        VkDescriptorSet GetDrawImageDescriptors() { return DrawImageDescriptors; }

        VkSampler GetSamplerLinear() { return DefaultSamplerLinear; }
        VkSampler GetSamplerNearest() { return DefaultSamplerNearest; }

    
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

        FDescriptorAllocatorGrowable GlobalDescriptorAllocator;

        std::vector<std::shared_ptr<FMeshAsset>> testMeshes;

        FDeletionQueue MainDeletionQueue;

        FGPUSceneData SceneData;

        FAllocatedImage WhiteImage;
        FAllocatedImage BlackImage;
        FAllocatedImage GreyImage;
        FAllocatedImage ErrorCheckerboardImage;

        VkSampler DefaultSamplerLinear;
        VkSampler DefaultSamplerNearest;


        FMaterialInstance defaultData;
        GLTFMetallicRoughness metalRoughMaterial;
        
        VkDescriptorSetLayout SingleImageDescriptorLayout;
        
        VkDescriptorSetLayout GpuSceneDataDescriptorLayout;

        VkDescriptorSet DrawImageDescriptors;
        VkDescriptorSetLayout DrawImageDescriptorLayout;

        VkPipeline GradientPipeline;
        VkPipelineLayout GradientPipelineLayout;
        

        VkPipeline MeshPipeline;
        VkPipelineLayout MeshPipelineLayout;
        
        bool bInitialized = false;
    };
}
