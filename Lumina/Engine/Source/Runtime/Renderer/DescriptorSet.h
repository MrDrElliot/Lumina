#pragma once

#include <memory>
#include <glm/glm.hpp>


namespace Lumina
{
    class FImageSampler;
    class FBuffer;
}

namespace Lumina
{
    class FImage;

    enum class EDescriptorBindingRate
    {
        SCENE = 0,
        PASS = 1,
        DRAW_CALL = 2
    };

    enum class EDescriptorBindingType : glm::uint32
    {
        SAMPLED_IMAGE,
        STORAGE_IMAGE,
        UNIFORM_BUFFER,
        STORAGE_BUFFER
    };

    enum class EDescriptorFlags : glm::uint32
    {
        PARTIALLY_BOUND = 1 << 0,
    };

    struct FDescriptorBinding
    {
        glm::uint32 Binding;
        EDescriptorBindingType Type;
        glm::uint32 ArrayCount;
        glm::uint64 Flags;
    };

    struct FDescriptorSetSpecification
    {
        std::vector<FDescriptorBinding> Bindings;
    };
    
    class FDescriptorSet
    {
    public:
        virtual ~FDescriptorSet() = default;

        static std::shared_ptr<FDescriptorSet> Create(const FDescriptorSetSpecification& InSpec);

        virtual void Destroy() = 0;

        virtual void Write(glm::uint16 Binding, glm::uint16 ArrayElement, std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler) = 0;
        virtual void Write(glm::uint16 Binding, glm::uint16 ArrayElement, std::shared_ptr<FBuffer> Buffer, glm::uint64 Size, glm::uint64 Offset) = 0;
        
    
    };
}
