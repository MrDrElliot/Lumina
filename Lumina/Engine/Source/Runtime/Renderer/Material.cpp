#include "Material.h"
#include "Pipeline.h"
#include "DescriptorSet.h"
#include "Renderer.h"
#include "Shader.h"
#include "RHI/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
    LMaterial::LMaterial(const TRefPtr<FPipeline>& InPipeline, const FMaterialTextures& InTextures, const FMaterialAttributes& InAttributes)
        : LAsset("Test"), Pipeline(InPipeline), Textures(InTextures), Attributes(std::move(InAttributes))
    {
        // Initialize descriptor bindings
        std::vector<FDescriptorBinding> Bindings;
        Bindings.emplace_back(1, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);
        Bindings.emplace_back(3, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);
        Bindings.emplace_back(4, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);
        Bindings.emplace_back(5, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);
        Bindings.emplace_back(6, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT);

        FDescriptorSetSpecification GlobalSetSpec = {};
        GlobalSetSpec.Bindings = std::move(Bindings);
        
        for (uint32 i = 0; i < FRenderer::GetConfig().FramesInFlight; i++)
        {
            auto Set = FDescriptorSet::Create(GlobalSetSpec);
            DescriptorSets.push_back(Set);
        }
    }

    LMaterial::~LMaterial()
    {
        for (auto& Set : DescriptorSets)
        {
           Set->Destroy(); 
        }
    }

    std::shared_ptr<LMaterial> LMaterial::Create(const TRefPtr<FPipeline>& InPipeline, const FMaterialTextures& Textures, const FMaterialAttributes& Attributes)
    {
        return std::make_shared<LMaterial>(InPipeline, Textures, Attributes);
    }

    void LMaterial::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler)
    {
        DescriptorSets[FRenderer::GetCurrentFrameIndex()]->Write(Binding, ArrayElement, Image, Sampler);
    }

    void LMaterial::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset)
    {
        DescriptorSets[FRenderer::GetCurrentFrameIndex()]->Write(Binding, ArrayElement, Buffer, Size, Offset);
    }

    void LMaterial::SetBaseColor(const glm::vec4& Color)
    {
        Attributes.BaseColor = Color;
    }

    void LMaterial::SetRoughness(float Roughness)
    {
        Attributes.Roughness = Roughness;
    }

    void LMaterial::SetEmissiveIntensity(float Intensity)
    {
        Attributes.EmissiveIntensity = Intensity;
    }

    void LMaterial::SetMetallic(float Metallic)
    {
        Attributes.Metallic = Metallic;
    }

    void LMaterial::Bind(const TRefPtr<FPipeline>& InPipeline, uint32 Index)
    {
        TRefPtr<FDescriptorSet> DescriptorSet = DescriptorSets[FRenderer::GetCurrentFrameIndex()];
        AssertMsg(DescriptorSet, "Tried to bind an invalid material descriptor set!");

        TFastVector<uint32> dynamicOffsets;
        FRenderer::BindSet(DescriptorSet, Pipeline, (uint8)Index, std::move(dynamicOffsets));
        
        Write(1, 0, Textures.BaseColor,        FRenderer::GetLinearSampler());
        Write(3, 0, Textures.Normal,           FRenderer::GetLinearSampler());
        Write(4, 0, Textures.MetallicRoughness,FRenderer::GetLinearSampler());
        Write(5, 0, Textures.Emissive,         FRenderer::GetLinearSampler());
        Write(6, 0, Textures.AmbientOcclusion, FRenderer::GetLinearSampler());
    }
}
