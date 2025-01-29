#pragma once

#include <glm/glm.hpp>

#include "RenderResource.h"
#include "Containers/Array.h"
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    enum class EShaderStage : uint8;
    class FImageSampler;
    class FBuffer;
    class FImage;

    enum class EDescriptorBindingRate
    {
        SCENE = 0,
        PASS = 1,
        DRAW_CALL = 2
    };

    enum class EDescriptorBindingType : uint32
    {
        SAMPLED_IMAGE,
        STORAGE_IMAGE,
        UNIFORM_BUFFER,
        UNIFORM_BUFFER_DYNAMIC,
        STORAGE_BUFFER,
        STORAGE_BUFFER_DYNAMIC,
    };

    enum class EDescriptorFlags : uint32
    {
        PARTIALLY_BOUND = 1 << 0,
    };
    
    struct FDescriptorBinding
    {
        uint32 Binding;               // The binding index
        EDescriptorBindingType Type;  // Type of descriptor (e.g., uniform buffer, texture)
        uint32 ArrayCount;            // Number of descriptors in the array
        uint64 Flags;                 // Additional flags for binding (e.g., PARTIALLY_BOUND)
        EShaderStage ShaderStage;     // Shader stage this binding applies to (VERTEX, FRAGMENT, etc.)
    };


    struct FDescriptorSetSpecification
    {
        std::vector<FDescriptorBinding> Bindings;
    };
    
    class FDescriptorSet : public FRenderResource
    {
    public:
        virtual ~FDescriptorSet() = default;

        static TRefPtr<FDescriptorSet> Create(const FDescriptorSetSpecification& InSpec);
        
        virtual void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset) = 0;
        virtual void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler) = 0;
        virtual void Write(uint16 Binding, uint16 ArrayElement, TVector<TRefPtr<FImage>> Images, TRefPtr<FImageSampler> Sampler) = 0;

    
    };
}
