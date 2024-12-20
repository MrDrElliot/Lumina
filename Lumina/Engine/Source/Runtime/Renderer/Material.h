#pragma once

#include <glm/glm.hpp>
#include "Memory/RefCounted.h"
#include "Image.h"
#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FBuffer;
    class FPipeline;

    enum class EMaterialFeatures : uint8
    {
        
    };

    struct alignas(16) FMaterialAttributes
    {
        
        glm::vec4 BaseColor = glm::vec4(1.0f);
        float Roughness = 1.0f;
        float Metallic = 0.5f;
        float EmissiveIntensity = 1.0f;
    };

    struct FMaterialTextures
    {
        TRefPtr<FImage> BaseColor;
        TRefPtr<FImage> Normal;
        TRefPtr<FImage> MetallicRoughness;
        TRefPtr<FImage> Emissive;
        TRefPtr<FImage> AmbientOcclusion;
    };
    

    class FDescriptorSet;
    
    class Material : public LAsset
    {
    public:

        Material(const TRefPtr<FPipeline>& InPipeline, const FMaterialTextures& InTextures, const FMaterialAttributes& InAttributes);
        ~Material();

        static TAssetHandle<Material> Create(const TRefPtr<FPipeline>& InPipeline, const FMaterialTextures& Textures, const FMaterialAttributes& Attributes);

        FMaterialAttributes& GetMaterialAttributes() { return Attributes; }
        
        virtual void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler);
        virtual void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset);

        virtual void SetBaseColor(const glm::vec4& Color);
        virtual void SetRoughness(float Roughness);
        virtual void SetEmissiveIntensity(float Intensity);
        virtual void SetMetallic(float Metallic);

        
        virtual void Bind(const TRefPtr<FPipeline>& Pipeline, uint32 Index = 0);


    private:

        TRefPtr<FPipeline> Pipeline;
        TFastVector<TRefPtr<FDescriptorSet>> DescriptorSets;
        FMaterialTextures Textures;
        FMaterialAttributes Attributes;
    
    };
}
