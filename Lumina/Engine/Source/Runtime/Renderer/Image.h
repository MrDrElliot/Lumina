#pragma once

#include <filesystem>
#include <glm/glm.hpp>

#include "PipelineStage.h"
#include "Source/Runtime/GUID/GUID.h"


namespace Lumina
{
    class FCommandBuffer;

    enum class EImageLayout
    {
        UNDEFINED = 0,
        GENERAL = 1,
        COLOR_ATTACHMENT = 2,
        DEPTH_STENCIL_ATTACHMENT = 3,
        DEPTH_STENCIL_READ_ONLY = 4,
        SHADER_READ_ONLY = 5,
        TRANSFER_SRC = 6,
        TRANSFER_DST = 7,
        PRESENT_SRC = 1000001002
    };

    enum class EImageUsage : glm::uint8
    {
        TEXTURE,
        RENDER_TARGET,
        DEPTH_BUFFER
    };

    enum class EImageFormat : glm::uint8
    {
        R8,
        RB16,
        RGB24,
        RGBA32_SRGB,
        RGBA32_UNORM,
        BGRA32_SRGB,
        BGRA32_UNORM,
        RGB32_HDR,
        RGBA64_HDR,
        RGBA128_HDR,
        D32,
        BC1,
        BC5,
        BC6h,
        BC7,
        RGBA64_SFLOAT,
        RGB24_UNORM
    };

    enum class EImageType : glm::uint8
    {
        TYPE_1D,
        TYPE_2D,
        TYPE_3D,
    };

    struct FImageSpecification
    {
        glm::uvec3 Extent;
        std::filesystem::path Path;
        std::vector<glm::uint8_t> Pixels;
        EImageFormat Format;
        EImageUsage Usage;
        EImageType Type;
        glm::uint8 ArrayLayers;
        glm::uint8 MipLevels;

        static FImageSpecification Default()
        {
            FImageSpecification spec;
            spec.Extent = { 0, 0, 0 };
            spec.Path = "";
            spec.Format = EImageFormat::RGBA32_SRGB;
            spec.Usage = EImageUsage::TEXTURE;
            spec.Type = EImageType::TYPE_2D;
            spec.MipLevels = 1;
            spec.ArrayLayers = 1;

            return spec;
        }
    };

    
    class FImage
    {
    public:
        

        static std::shared_ptr<FImage> Create(const FImageSpecification& Spec);

        virtual ~FImage() = default;
        virtual void Destroy() = 0;
        virtual FImageSpecification GetSpecification() const = 0;
        FGuid GetGuid() { return Guid; }


        virtual void SetLayout(
            std::shared_ptr<FCommandBuffer> CmdBuffer,
            EImageLayout NewLayout,
            EPipelineStage SrcStage,
            EPipelineStage DstStage,
            EPipelineAccess SrcAccess = EPipelineAccess::NONE,
            EPipelineAccess DstAccess = EPipelineAccess::NONE
         ) = 0;

    private:

    protected:
        
        FGuid Guid;
        
    };



    
    enum class ESamplerFilteringMode : glm::uint32
    {
        NEAREST,
        LINEAR
    };

    enum class ESamplerAddressMode : glm::uint32
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP,
        CLAMP_BORDER
    };

    struct FImageSamplerSpecification
    {
        ESamplerFilteringMode MinFilteringMode;
        ESamplerFilteringMode MagFilteringMode;
        ESamplerFilteringMode MipMapFilteringMode;
        ESamplerAddressMode AddressMode;
        float MinLOD;
        float MaxLOD;
        float LODBias;
        glm::uint8 AnisotropicFilteringLevel;
    };

    class FImageSampler
    {
    public:
        virtual ~FImageSampler() = default;

        static std::shared_ptr<FImageSampler> Create(const FImageSamplerSpecification& Spec);

        virtual void Destroy() = 0;
    };
}
