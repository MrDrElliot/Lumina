#include "RenderResource.h"

#include "RenderContext.h"
#include "Containers/Array.h"


namespace Lumina
{

    // DLL Template specialization exports.
    template class LUMINA_API TRefCountPtr<IRHIResource>;
    template class LUMINA_API TRefCountPtr<IEventQuery>;
    template class LUMINA_API TRefCountPtr<FRHIBuffer>;
    template class LUMINA_API TRefCountPtr<FRHIImage>;
    template class LUMINA_API TRefCountPtr<FRHISampler>;
    template class LUMINA_API TRefCountPtr<FRHIShader>;
    template class LUMINA_API TRefCountPtr<FRHIVertexShader>;
    template class LUMINA_API TRefCountPtr<FRHIPixelShader>;
    template class LUMINA_API TRefCountPtr<FRHIComputeShader>;
    template class LUMINA_API TRefCountPtr<ICommandList>;
    template class LUMINA_API TRefCountPtr<FRHIViewport>;
    template class LUMINA_API TRefCountPtr<FRHIGraphicsPipeline>;
    template class LUMINA_API TRefCountPtr<FRHIComputePipeline>;
    template class LUMINA_API TRefCountPtr<FRHIBindingLayout>;
    template class LUMINA_API TRefCountPtr<FRHIBindingSet>;
    template class LUMINA_API TRefCountPtr<IRHIInputLayout>;
    template class LUMINA_API TRefCountPtr<FShaderLibrary>;
    template class LUMINA_API TRefCountPtr<FRHIDescriptorTable>;

    SIZE_T GTotalRenderResourcesAllocated = 0;
    
    IRHIResource::IRHIResource()
    {
        GTotalRenderResourcesAllocated++;
    }

    IRHIResource::~IRHIResource()
    {
        GTotalRenderResourcesAllocated--;
    }

    void IRHIResource::Destroy() const
    {
        if (!AtomicFlags.MarkForDelete(std::memory_order_relaxed))
        {
            Memory::Delete(this);
        }
    }
    
    void FRHIViewport::SetSize(const FIntVector2D& InSize)
    {
        if (Size == InSize)
        {
            return;
        }
        
        CreateRenderTarget(InSize);
    }

    void FRHIViewport::CreateRenderTarget(const FIntVector2D& InSize)
    {
        FRHIImageDesc Desc;
        Desc.Format = EFormat::BGRA8_UNORM;
        Desc.Flags.SetMultipleFlags(EImageCreateFlags::RenderTarget, EImageCreateFlags::ShaderResource);
        Desc.Extent = InSize;
        Desc.InitialState = EResourceStates::RenderTarget;
        Desc.bKeepInitialState = true;
        Desc.DebugName = "Viewport Render Target";

        RenderTarget = RenderContext->CreateImage(Desc);
        
        RenderContext->SetObjectName(RenderTarget, "Viewport Render Target", EAPIResourceType::Image);
    }

    FTextureSlice FTextureSlice::Resolve(const FRHIImageDesc& desc) const
    {
        FTextureSlice ret(*this);

        assert(MipLevel < desc.NumMips);

        if (X == uint32(-1))
        {
            ret.X = std::max((uint32)desc.Extent.X >> MipLevel, 1u);
        }

        if (Y == uint32(-1))
            ret.Y = std::max((uint32)desc.Extent.Y >> MipLevel, 1u);

        if (Z == uint32(-1))
        {
            if (desc.Dimension == EImageDimension::Texture3D)
                ret.Z = std::max((uint32)desc.Depth >> MipLevel, 1u);
            else
                ret.Z = 1;
        }

        return ret;
    }

    FTextureSubresourceSet FTextureSubresourceSet::Resolve(const FRHIImageDesc& Desc, bool bSingleMipLevel) const
    {
        FTextureSubresourceSet ret;
        ret.BaseMipLevel = BaseMipLevel;

        if (bSingleMipLevel)
        {
            ret.NumMipLevels = 1;
        }
        else
        {
            int lastMipLevelPlusOne = std::min(BaseMipLevel + NumMipLevels, (uint32)Desc.NumMips);
            ret.NumMipLevels = uint32(std::max(0u, lastMipLevelPlusOne - BaseMipLevel));
        }

        switch (Desc.Dimension)  // NOLINT(clang-diagnostic-switch-enum)
        {
        default: 
            ret.BaseArraySlice = 0;
            ret.NumArraySlices = 1;
            break;
        }

        return ret;
    }

    bool FTextureSubresourceSet::IsEntireTexture(const FRHIImageDesc& Desc) const
    {
        if (BaseMipLevel > 0u || BaseMipLevel + NumMipLevels < Desc.NumMips)
        {
            return false;
        }
        return true;
    }
}

 // Format mapping table. The rows must be in the exactly same order as Format enum members are defined.
    static const FFormatInfo c_FormatInfo[] = {
    //        format                   name             bytes blk         kind               red   green   blue  alpha  depth  stencl signed  srgb
        { EFormat::UNKNOWN,           "UNKNOWN",           0,   0, EFormatKind::Integer,      false, false, false, false, false, false, false, false },
        { EFormat::R8_UINT,           "R8_UINT",           1,   1, EFormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { EFormat::R8_SINT,           "R8_SINT",           1,   1, EFormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { EFormat::R8_UNORM,          "R8_UNORM",          1,   1, EFormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { EFormat::R8_SNORM,          "R8_SNORM",          1,   1, EFormatKind::Normalized,   true,  false, false, false, false, false, true,  false },
        { EFormat::RG8_UINT,          "RG8_UINT",          2,   1, EFormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { EFormat::RG8_SINT,          "RG8_SINT",          2,   1, EFormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { EFormat::RG8_UNORM,         "RG8_UNORM",         2,   1, EFormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { EFormat::RG8_SNORM,         "RG8_SNORM",         2,   1, EFormatKind::Normalized,   true,  true,  false, false, false, false, true,  false },
        { EFormat::R16_UINT,          "R16_UINT",          2,   1, EFormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { EFormat::R16_SINT,          "R16_SINT",          2,   1, EFormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { EFormat::R16_UNORM,         "R16_UNORM",         2,   1, EFormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { EFormat::R16_SNORM,         "R16_SNORM",         2,   1, EFormatKind::Normalized,   true,  false, false, false, false, false, true,  false },
        { EFormat::R16_FLOAT,         "R16_FLOAT",         2,   1, EFormatKind::Float,        true,  false, false, false, false, false, true,  false },
        { EFormat::BGRA4_UNORM,       "BGRA4_UNORM",       2,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::B5G6R5_UNORM,      "B5G6R5_UNORM",      2,   1, EFormatKind::Normalized,   true,  true,  true,  false, false, false, false, false },
        { EFormat::B5G5R5A1_UNORM,    "B5G5R5A1_UNORM",    2,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::RGBA8_UINT,        "RGBA8_UINT",        4,   1, EFormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { EFormat::RGBA8_SINT,        "RGBA8_SINT",        4,   1, EFormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { EFormat::RGBA8_UNORM,       "RGBA8_UNORM",       4,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::RGBA8_SNORM,       "RGBA8_SNORM",       4,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, true,  false },
        { EFormat::BGRA8_UNORM,       "BGRA8_UNORM",       4,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::SRGBA8_UNORM,      "SRGBA8_UNORM",      4,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { EFormat::SBGRA8_UNORM,      "SBGRA8_UNORM",      4,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::R11G11B10_FLOAT,   "R11G11B10_FLOAT",   4,   1, EFormatKind::Float,        true,  true,  true,  false, false, false, false, false },
        { EFormat::RG16_UINT,         "RG16_UINT",         4,   1, EFormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { EFormat::RG16_SINT,         "RG16_SINT",         4,   1, EFormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { EFormat::RG16_UNORM,        "RG16_UNORM",        4,   1, EFormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { EFormat::RG16_SNORM,        "RG16_SNORM",        4,   1, EFormatKind::Normalized,   true,  true,  false, false, false, false, true,  false },
        { EFormat::RG16_FLOAT,        "RG16_FLOAT",        4,   1, EFormatKind::Float,        true,  true,  false, false, false, false, true,  false },
        { EFormat::R32_UINT,          "R32_UINT",          4,   1, EFormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { EFormat::R32_SINT,          "R32_SINT",          4,   1, EFormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { EFormat::R32_FLOAT,         "R32_FLOAT",         4,   1, EFormatKind::Float,        true,  false, false, false, false, false, true,  false },
        { EFormat::RGBA16_UINT,       "RGBA16_UINT",       8,   1, EFormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { EFormat::RGBA16_SINT,       "RGBA16_SINT",       8,   1, EFormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { EFormat::RGBA16_FLOAT,      "RGBA16_FLOAT",      8,   1, EFormatKind::Float,        true,  true,  true,  true,  false, false, true,  false },
        { EFormat::RGBA16_UNORM,      "RGBA16_UNORM",      8,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::RGBA16_SNORM,      "RGBA16_SNORM",      8,   1, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, true,  false },
        { EFormat::RG32_UINT,         "RG32_UINT",         8,   1, EFormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { EFormat::RG32_SINT,         "RG32_SINT",         8,   1, EFormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { EFormat::RG32_FLOAT,        "RG32_FLOAT",        8,   1, EFormatKind::Float,        true,  true,  false, false, false, false, true,  false },
        { EFormat::RGB32_UINT,        "RGB32_UINT",        12,  1, EFormatKind::Integer,      true,  true,  true,  false, false, false, false, false },
        { EFormat::RGB32_SINT,        "RGB32_SINT",        12,  1, EFormatKind::Integer,      true,  true,  true,  false, false, false, true,  false },
        { EFormat::RGB32_FLOAT,       "RGB32_FLOAT",       12,  1, EFormatKind::Float,        true,  true,  true,  false, false, false, true,  false },
        { EFormat::RGBA32_UINT,       "RGBA32_UINT",       16,  1, EFormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { EFormat::RGBA32_SINT,       "RGBA32_SINT",       16,  1, EFormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { EFormat::RGBA32_FLOAT,      "RGBA32_FLOAT",      16,  1, EFormatKind::Float,        true,  true,  true,  true,  false, false, true,  false },
        { EFormat::D16,               "D16",               2,   1, EFormatKind::DepthStencil, false, false, false, false, true,  false, false, false },
        { EFormat::D24S8,             "D24S8",             4,   1, EFormatKind::DepthStencil, false, false, false, false, true,  true,  false, false },
        { EFormat::X24G8_UINT,        "X24G8_UINT",        4,   1, EFormatKind::Integer,      false, false, false, false, false, true,  false, false },
        { EFormat::D32,               "D32",               4,   1, EFormatKind::DepthStencil, false, false, false, false, true,  false, false, false },
        { EFormat::D32S8,             "D32S8",             8,   1, EFormatKind::DepthStencil, false, false, false, false, true,  true,  false, false },
        { EFormat::X32G8_UINT,        "X32G8_UINT",        8,   1, EFormatKind::Integer,      false, false, false, false, false, true,  false, false },
        { EFormat::BC1_UNORM,         "BC1_UNORM",         8,   4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::BC1_UNORM_SRGB,    "BC1_UNORM_SRGB",    8,   4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { EFormat::BC2_UNORM,         "BC2_UNORM",         16,  4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::BC2_UNORM_SRGB,    "BC2_UNORM_SRGB",    16,  4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { EFormat::BC3_UNORM,         "BC3_UNORM",         16,  4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::BC3_UNORM_SRGB,    "BC3_UNORM_SRGB",    16,  4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { EFormat::BC4_UNORM,         "BC4_UNORM",         8,   4, EFormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { EFormat::BC4_SNORM,         "BC4_SNORM",         8,   4, EFormatKind::Normalized,   true,  false, false, false, false, false, true,  false },
        { EFormat::BC5_UNORM,         "BC5_UNORM",         16,  4, EFormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { EFormat::BC5_SNORM,         "BC5_SNORM",         16,  4, EFormatKind::Normalized,   true,  true,  false, false, false, false, true,  false },
        { EFormat::BC6H_UFLOAT,       "BC6H_UFLOAT",       16,  4, EFormatKind::Float,        true,  true,  true,  false, false, false, false, false },
        { EFormat::BC6H_SFLOAT,       "BC6H_SFLOAT",       16,  4, EFormatKind::Float,        true,  true,  true,  false, false, false, true,  false },
        { EFormat::BC7_UNORM,         "BC7_UNORM",         16,  4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { EFormat::BC7_UNORM_SRGB,    "BC7_UNORM_SRGB",    16,  4, EFormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
    };


const FFormatInfo& GetFormatInfo(EFormat format)
{
    static_assert(sizeof(c_FormatInfo) / sizeof(FFormatInfo) == size_t(EFormat::COUNT), 
        "The format info table doesn't have the right number of elements");

    if (uint32(format) >= uint32(EFormat::COUNT))
    {
        return c_FormatInfo[0];
    }

    const FFormatInfo& info = c_FormatInfo[uint32(format)];
    Assert(info.Format == format)
    return info;
}
