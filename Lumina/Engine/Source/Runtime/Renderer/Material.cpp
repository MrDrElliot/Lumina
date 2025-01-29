#include "Material.h"
#include "Pipeline.h"
#include "DescriptorSet.h"
#include "Renderer.h"
#include "Shader.h"
#include "RHI/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
    FMaterial::FMaterial(const TRefPtr<FPipeline>& InPipeline)
        : Pipeline(InPipeline)
    {
        constexpr uint32 MaxTextures = 1024; 
        std::vector<FDescriptorBinding> Bindings;
        Bindings.emplace_back(1, EDescriptorBindingType::SAMPLED_IMAGE, MaxTextures, (uint32)EDescriptorFlags::PARTIALLY_BOUND, EShaderStage::FRAGMENT);

        FDescriptorSetSpecification GlobalSetSpec = {};
        GlobalSetSpec.Bindings = std::move(Bindings);
        
        for (uint32 i = 0; i < FRenderer::GetConfig().FramesInFlight; i++)
        {
            auto Set = FDescriptorSet::Create(GlobalSetSpec);
            Set->SetFriendlyName("Material: " + eastl::to_string(i));
            DescriptorSets.PushBack(Set);
        }
    }

    FMaterial::~FMaterial()
    {
        DescriptorSets.clear();
    }

    TRefPtr<FMaterial> FMaterial::Create(const TRefPtr<FPipeline>& InPipeline)
    {
        return MakeRefPtr<FMaterial>(InPipeline);
    }

    void FMaterial::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler)
    {
        if (Image.IsValid())
        {
            DescriptorSets[FRenderer::GetCurrentFrameIndex()]->Write(Binding, ArrayElement, Image, Sampler);
        }
    }

    void FMaterial::Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset)
    {
        DescriptorSets[FRenderer::GetCurrentFrameIndex()]->Write(Binding, ArrayElement, Buffer, Size, Offset);
    }

    void FMaterial::Bind(const TRefPtr<FPipeline>& InPipeline, uint32 Index)
    {
        TRefPtr<FDescriptorSet> DescriptorSet = DescriptorSets[FRenderer::GetCurrentFrameIndex()];
        AssertMsg(DescriptorSet, "Tried to bind an invalid material descriptor set!");

        TVector<uint32> dynamicOffsets;
        FRenderer::BindSet(DescriptorSet, Pipeline, (uint8)Index, std::move(dynamicOffsets));
    }
}
