#pragma once
#include "VulkanTypes.h"
#include "Source/Runtime/Renderer/Material.h"

namespace Lumina
{
    enum class EMaterialPass : uint8_t
    {
        MainColor,
        Transparent,
        Other,
    };

    struct FMaterialPipeline
    {
        VkPipeline Pipeline;
        VkPipelineLayout Layout;
    };

    struct FMaterialInstance
    {
        FMaterialPipeline* Pipeline;
        VkDescriptorSet MaterialSet;
        EMaterialPass PassType;
    };

    struct GLTFMetallicRoughness
    {
        FMaterialPipeline OpaquePipeline;
        FMaterialPipeline TransparentPipeline;

        VkDescriptorSetLayout MaterialLayout;

        struct Constants
        {
            glm::vec4 Color;
            glm::vec4 MetalRoughness;

            glm::vec4 Extra[14];
        };

        struct Resources
        {
            FAllocatedImage ColorImage;
            VkSampler ColorSampler;
            FAllocatedImage MetalRoughImage;
            VkSampler MetalRoughSampler;
            VkBuffer DataBuffer;
            uint32_t DataBufferOffset;
        };

        FDescriptorWriter Writer;

        void BuildPipelines();
        void ClearResources(VkDevice InDevice);

        FMaterialInstance WriteMaterial(VkDevice InDevice, EMaterialPass InPass, const Resources& Resources, FDescriptorAllocatorGrowable& Descriptor);
    };

    
    class FVulkanMaterial : public FMaterial
    {
    public:

        
    
    };
}
