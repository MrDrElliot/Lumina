#pragma once

#include <filesystem>
#include <glm/glm.hpp>

#include "PipelineStage.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "GUID/GUID.h"
#include "Memory/RefCounted.h"


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
        DEPTH_BUFFER,
        RESOLVE,
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

    enum class EImageSampleCount : uint8
    {
        ONE,
        FOUR,
    };

    struct FImageSpecification
    {
        std::string DebugName;
        glm::uvec3 Extent;
        TFastVector<uint8> Pixels;
        uint32 MipLevels;
        int32 SourceChannels;
        EImageFormat Format;
        EImageSampleCount SampleCount;
        EImageUsage Usage;
        EImageType Type;
        uint8 ArrayLayers;

        friend FArchive& operator << (FArchive& Ar, FImageSpecification& data)
        {
            Ar << data.DebugName;
            Ar << data.Extent.x << data.Extent.y << data.Extent.z;
            Ar << data.Pixels;
            Ar << data.MipLevels;
            Ar << data.SourceChannels;
            Ar << data.Format;
            Ar << data.SampleCount;
            Ar << data.Usage;
            Ar << data.Type;
            Ar << data.ArrayLayers;

            return Ar;
        }

        static FImageSpecification Default()
        {
            FImageSpecification spec;
            spec.DebugName = "";
            spec.Extent = { 0, 0, 0 };
            spec.Format = EImageFormat::RGBA32_SRGB;
            spec.Usage = EImageUsage::TEXTURE;
            spec.Type = EImageType::TYPE_2D;
            spec.MipLevels = 1;
            spec.ArrayLayers = 1;

            return spec;
        }
    };

    
    class FImage : public RefCounted
    {
    public:
        

        static TRefPtr<FImage> Create(const FImageSpecification& Spec);

        virtual ~FImage() = default;
        virtual void Destroy() = 0;
        virtual FImageSpecification GetSpecification() const = 0;
        FGuid GetGuid() { return Guid; }


        virtual void SetLayout(
            TRefPtr<FCommandBuffer> CmdBuffer,
            EImageLayout NewLayout,
            EPipelineStage SrcStage,
            EPipelineStage DstStage,
            EPipelineAccess SrcAccess = EPipelineAccess::NONE,
            EPipelineAccess DstAccess = EPipelineAccess::NONE
         ) = 0;

    private:

        LString DebugName;
        
    protected:
        
        FGuid Guid;
        
    };

    
    enum class ESamplerFilteringMode : uint32
    {
        NEAREST,
        LINEAR
    };

    enum class ESamplerAddressMode : uint32
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
        uint8 AnisotropicFilteringLevel;
    };

    class FImageSampler : public RefCounted
    {
    public:
        virtual ~FImageSampler() = default;

        static TRefPtr<FImageSampler> Create(const FImageSamplerSpecification& Spec);

        virtual void Destroy() = 0;
    };
}
