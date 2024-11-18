#include "Material.h"
#include "Pipeline.h"
#include "DescriptorSet.h"
#include "Renderer.h"
#include "Shader.h"
#include "RHI/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
    Material::Material(const TRefPtr<FPipeline>& InPipeline, const FMaterialTextures& InTextures, const FMaterialAttributes& InAttributes)
        : LAsset("Test"), Pipeline(InPipeline), Textures(InTextures), Attributes(std::move(InAttributes))
    {
        // Initialize descriptor bindings
        std::vector<FDescriptorBinding> Bindings;

        // Binding 1: Albedo texture (sampler)
        Bindings.emplace_back(1, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);

        // Binding 3 (Normal map) 
        Bindings.emplace_back(3, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);

        Bindings.emplace_back(4, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);

        Bindings.emplace_back(5, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);
        Bindings.emplace_back(6, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);


        // Create descriptor set specification
        FDescriptorSetSpecification GlobalSetSpec = {};
        GlobalSetSpec.Bindings = std::move(Bindings);

        uint32 FramesInFlight = FRenderer::GetConfig().FramesInFlight;
        // Create descriptor sets for each frame
        for (uint32 i = 0; i < FramesInFlight; i++)
        {
            // Create the descriptor set using the specification
            auto Set = FDescriptorSet::Create(GlobalSetSpec);

            // Add the created set to the list of descriptor sets
            DescriptorSets.push_back(Set);
        }
    }

    Material::~Material()
    {
        for (auto& Set : DescriptorSets)
        {
           Set->Destroy(); 
        }
    }

    TAssetHandle<Material> Material::Create(const TRefPtr<FPipeline>& InPipeline, const FMaterialTextures& Textures, const FMaterialAttributes& Attributes)
    {
        return TAssetHandle<Material>(FGuid::Generate());
        //return MakeRefPtr<Material>(InPipeline, Textures, Attributes);
    }

    void Material::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler)
    {
        DescriptorSets[FRenderer::GetCurrentFrameIndex()]->Write(Binding, ArrayElement, Image, Sampler);
    }

    void Material::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset)
    {
        DescriptorSets[FRenderer::GetCurrentFrameIndex()]->Write(Binding, ArrayElement, Buffer, Size, Offset);
    }

    void Material::SetBaseColor(const glm::vec4& Color)
    {
        Attributes.BaseColor = Color;
    }

    void Material::SetRoughness(float Roughness)
    {
        Attributes.Roughness = Roughness;
    }

    void Material::SetEmissiveIntensity(float Intensity)
    {
        Attributes.EmissiveIntensity = Intensity;
    }

    void Material::SetMetallic(float Metallic)
    {
        Attributes.Metallic = Metallic;
    }

    void Material::Bind(const TRefPtr<FPipeline>& InPipeline, uint32 Index)
    {
        TRefPtr<FDescriptorSet> DescriptorSet = DescriptorSets[FRenderer::GetCurrentFrameIndex()];
        AssertMsg(DescriptorSet, "Tried to bind an invalid material descriptor set!");

        TFastVector<uint32> dynamicOffsets;
        dynamicOffsets.clear();
        FRenderer::BindSet(DescriptorSet, Pipeline, (uint8)Index, std::move(dynamicOffsets));
        
        Write(1, 0, Textures.BaseColor,        FRenderer::GetLinearSampler());
        Write(3, 0, Textures.Normal,           FRenderer::GetLinearSampler());
        Write(4, 0, Textures.MetallicRoughness,FRenderer::GetLinearSampler());
        Write(5, 0, Textures.Emissive,         FRenderer::GetLinearSampler());
        Write(6, 0, Textures.AmbientOcclusion, FRenderer::GetLinearSampler());
    }
}
