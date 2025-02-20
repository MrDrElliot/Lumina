#pragma once

#include <glm/glm.hpp>
#include "PipelineStage.h"
#include "RenderResource.h"
#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "GUID/GUID.h"


namespace Lumina
{
    class FCommandBuffer;

    
    struct FImageSpecification
    {
        FString DebugName;
        glm::uvec3 Extent;
        TVector<uint8> Pixels;
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
            spec.SampleCount = EImageSampleCount::ONE;
            spec.Type = EImageType::TYPE_2D;
            spec.MipLevels = 1;
            spec.ArrayLayers = 1;

            return spec;
        }
    };

    
    class FImage : public IRenderResource
    {
    public:
        

        static FRHIImage Create(const FImageSpecification& Spec);

        virtual ~FImage() = default;
        virtual FImageSpecification GetSpecification() const = 0;
        FGuid GetGuid() { return Guid; }
    

        virtual void SetLayout(
            FRHICommandBuffer CmdBuffer,
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

    class FImageSampler : public IRenderResource
    {
    public:
        
        virtual ~FImageSampler() = default;

        static FRHIImageSampler Create(const FImageSamplerSpecification& Spec);
        
        virtual void Destroy() = 0;
    };
}
