#include "VulkanResources.h"

#include "Renderer/RenderResource.h"
#include "VulkanCommandList.h"
#include "VulkanMacros.h"

namespace Lumina
{
    struct FormatMapping
    {
        EFormat Format;
        VkFormat vkFormat;
    };

    static const std::array<FormatMapping, size_t(EFormat::COUNT)> FormatMap = { {
        { EFormat::UNKNOWN,           VK_FORMAT_UNDEFINED                },
        { EFormat::R8_UINT,           VK_FORMAT_R8_UINT                  },
        { EFormat::R8_SINT,           VK_FORMAT_R8_SINT                  },
        { EFormat::R8_UNORM,          VK_FORMAT_R8_UNORM                 },
        { EFormat::R8_SNORM,          VK_FORMAT_R8_SNORM                 },
        { EFormat::RG8_UINT,          VK_FORMAT_R8G8_UINT                },
        { EFormat::RG8_SINT,          VK_FORMAT_R8G8_SINT                },
        { EFormat::RG8_UNORM,         VK_FORMAT_R8G8_UNORM               },
        { EFormat::RG8_SNORM,         VK_FORMAT_R8G8_SNORM               },
        { EFormat::R16_UINT,          VK_FORMAT_R16_UINT                 },
        { EFormat::R16_SINT,          VK_FORMAT_R16_SINT                 },
        { EFormat::R16_UNORM,         VK_FORMAT_R16_UNORM                },
        { EFormat::R16_SNORM,         VK_FORMAT_R16_SNORM                },
        { EFormat::R16_FLOAT,         VK_FORMAT_R16_SFLOAT               },
        { EFormat::BGRA4_UNORM,       VK_FORMAT_B4G4R4A4_UNORM_PACK16    },
        { EFormat::B5G6R5_UNORM,      VK_FORMAT_B5G6R5_UNORM_PACK16      },
        { EFormat::B5G5R5A1_UNORM,    VK_FORMAT_B5G5R5A1_UNORM_PACK16    },
        { EFormat::RGBA8_UINT,        VK_FORMAT_R8G8B8A8_UINT            },
        { EFormat::RGBA8_SINT,        VK_FORMAT_R8G8B8A8_SINT            },
        { EFormat::RGBA8_UNORM,       VK_FORMAT_R8G8B8A8_UNORM           },
        { EFormat::RGBA8_SNORM,       VK_FORMAT_R8G8B8A8_SNORM           },
        { EFormat::BGRA8_UNORM,       VK_FORMAT_B8G8R8A8_UNORM           },
        { EFormat::SRGBA8_UNORM,      VK_FORMAT_R8G8B8A8_SRGB            },
        { EFormat::SBGRA8_UNORM,      VK_FORMAT_B8G8R8A8_SRGB            },
        { EFormat::R10G10B10A2_UNORM, VK_FORMAT_A2B10G10R10_UNORM_PACK32 },
        { EFormat::R11G11B10_FLOAT,   VK_FORMAT_B10G11R11_UFLOAT_PACK32  },
        { EFormat::RG16_UINT,         VK_FORMAT_R16G16_UINT              },
        { EFormat::RG16_SINT,         VK_FORMAT_R16G16_SINT              },
        { EFormat::RG16_UNORM,        VK_FORMAT_R16G16_UNORM             },
        { EFormat::RG16_SNORM,        VK_FORMAT_R16G16_SNORM             },
        { EFormat::RG16_FLOAT,        VK_FORMAT_R16G16_SFLOAT            },
        { EFormat::R32_UINT,          VK_FORMAT_R32_UINT                 },
        { EFormat::R32_SINT,          VK_FORMAT_R32_SINT                 },
        { EFormat::R32_FLOAT,         VK_FORMAT_R32_SFLOAT               },
        { EFormat::RGBA16_UINT,       VK_FORMAT_R16G16B16A16_UINT        },
        { EFormat::RGBA16_SINT,       VK_FORMAT_R16G16B16A16_SINT        },
        { EFormat::RGBA16_FLOAT,      VK_FORMAT_R16G16B16A16_SFLOAT      },
        { EFormat::RGBA16_UNORM,      VK_FORMAT_R16G16B16A16_UNORM       },
        { EFormat::RGBA16_SNORM,      VK_FORMAT_R16G16B16A16_SNORM       },
        { EFormat::RG32_UINT,         VK_FORMAT_R32G32_UINT              },
        { EFormat::RG32_SINT,         VK_FORMAT_R32G32_SINT              },
        { EFormat::RG32_FLOAT,        VK_FORMAT_R32G32_SFLOAT            },
        { EFormat::RGB32_UINT,        VK_FORMAT_R32G32B32_UINT           },
        { EFormat::RGB32_SINT,        VK_FORMAT_R32G32B32_SINT           },
        { EFormat::RGB32_FLOAT,       VK_FORMAT_R32G32B32_SFLOAT         },
        { EFormat::RGBA32_UINT,       VK_FORMAT_R32G32B32A32_UINT        },
        { EFormat::RGBA32_SINT,       VK_FORMAT_R32G32B32A32_SINT        },
        { EFormat::RGBA32_FLOAT,      VK_FORMAT_R32G32B32A32_SFLOAT      },
        { EFormat::D16,               VK_FORMAT_D16_UNORM                },
        { EFormat::D24S8,             VK_FORMAT_D24_UNORM_S8_UINT        },
        { EFormat::X24G8_UINT,        VK_FORMAT_D24_UNORM_S8_UINT        },
        { EFormat::D32,               VK_FORMAT_D32_SFLOAT               },
        { EFormat::D32S8,             VK_FORMAT_D32_SFLOAT_S8_UINT       },
        { EFormat::X32G8_UINT,        VK_FORMAT_D32_SFLOAT_S8_UINT       },
        { EFormat::BC1_UNORM,         VK_FORMAT_BC1_RGBA_UNORM_BLOCK     },
        { EFormat::BC1_UNORM_SRGB,    VK_FORMAT_BC1_RGBA_SRGB_BLOCK      },
        { EFormat::BC2_UNORM,         VK_FORMAT_BC2_UNORM_BLOCK          },
        { EFormat::BC2_UNORM_SRGB,    VK_FORMAT_BC2_SRGB_BLOCK           },
        { EFormat::BC3_UNORM,         VK_FORMAT_BC3_UNORM_BLOCK          },
        { EFormat::BC3_UNORM_SRGB,    VK_FORMAT_BC3_SRGB_BLOCK           },
        { EFormat::BC4_UNORM,         VK_FORMAT_BC4_UNORM_BLOCK          },
        { EFormat::BC4_SNORM,         VK_FORMAT_BC4_SNORM_BLOCK          },
        { EFormat::BC5_UNORM,         VK_FORMAT_BC5_UNORM_BLOCK          },
        { EFormat::BC5_SNORM,         VK_FORMAT_BC5_SNORM_BLOCK          },
        { EFormat::BC6H_UFLOAT,       VK_FORMAT_BC6H_UFLOAT_BLOCK        },
        { EFormat::BC6H_SFLOAT,       VK_FORMAT_BC6H_SFLOAT_BLOCK        },
        { EFormat::BC7_UNORM,         VK_FORMAT_BC7_UNORM_BLOCK          },
        { EFormat::BC7_UNORM_SRGB,    VK_FORMAT_BC7_SRGB_BLOCK           },

    } };

    VkFormat ConvertFormat(EFormat format)
    {
        Assert(format < EFormat::COUNT);
        Assert(FormatMap[uint32_t(format)].Format == format);

        return FormatMap[uint32_t(format)].vkFormat;
    }

    VkFormat GetVkFormat(EImageFormat Format)
    {
        switch (Format)
        {
            case EImageFormat::R8_UNORM:        return VK_FORMAT_R8_UNORM;
            case EImageFormat::R8_SNORM:        return VK_FORMAT_R8_SNORM;
            case EImageFormat::RG16_UNORM:      return VK_FORMAT_R16G16_UNORM;
            case EImageFormat::RGBA32_UNORM:    return VK_FORMAT_R8G8B8A8_UNORM;
            case EImageFormat::BGRA32_UNORM:    return VK_FORMAT_B8G8R8A8_UNORM;
            case EImageFormat::RGBA32_SRGB:     return VK_FORMAT_R8G8B8A8_SRGB;
            case EImageFormat::BGRA32_SRGB:     return VK_FORMAT_B8G8R8A8_SRGB;
            case EImageFormat::RGB32_SFLOAT:    return VK_FORMAT_R32G32B32_SFLOAT;
            case EImageFormat::RGBA64_SFLOAT:   return VK_FORMAT_R16G16B16A16_SFLOAT;
            case EImageFormat::RGBA128_SFLOAT:  return VK_FORMAT_R32G32B32A32_SFLOAT;
            case EImageFormat::D32:             return VK_FORMAT_D32_SFLOAT;
            case EImageFormat::BC1_UNORM:       return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
            case EImageFormat::BC1_SRGB:        return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
            case EImageFormat::BC3_UNORM:       return VK_FORMAT_BC3_UNORM_BLOCK;
            case EImageFormat::BC3_SRGB:        return VK_FORMAT_BC3_SRGB_BLOCK;
            case EImageFormat::BC5_UNORM:       return VK_FORMAT_BC5_UNORM_BLOCK;
            case EImageFormat::BC5_SNORM:       return VK_FORMAT_BC5_SNORM_BLOCK;
            case EImageFormat::BC6H_UFLOAT:     return VK_FORMAT_BC6H_UFLOAT_BLOCK;
            case EImageFormat::BC6H_SFLOAT:     return VK_FORMAT_BC6H_SFLOAT_BLOCK;
            case EImageFormat::BC7_UNORM:       return VK_FORMAT_BC7_UNORM_BLOCK;
            case EImageFormat::BC7_SRGB:        return VK_FORMAT_BC7_SRGB_BLOCK;
            default: return VK_FORMAT_UNDEFINED;
        }
    }

    
    VkBufferUsageFlags ToVkBufferUsage(TBitFlags<EBufferUsageFlags> Usage) 
    {
        VkBufferUsageFlags result = 0;

        if (Usage.IsFlagSet(EBufferUsageFlags::VertexBuffer))
        {
            result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
    
        if (Usage.IsFlagSet(EBufferUsageFlags::IndexBuffer))
        {
            result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
    
        if (Usage.IsFlagSet(EBufferUsageFlags::UniformBuffer))
        {
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::StorageBuffer))
        {
            result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::SourceCopy))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::StagingBuffer))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if (Usage.IsFlagSet(EBufferUsageFlags::CPUWritable))
        {
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        
        return result;
    }

    constexpr VkPrimitiveTopology ToVkPrimitiveTopology(EPrimitiveType PrimType)
    {
        switch (PrimType)
        {
            case EPrimitiveType::PointList:                     return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case EPrimitiveType::LineList:                      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case EPrimitiveType::LineStrip:                     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case EPrimitiveType::TriangleList:                  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case EPrimitiveType::TriangleStrip:                 return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case EPrimitiveType::TriangleFan:                   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case EPrimitiveType::TriangleListWithAdjacency:     return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
            case EPrimitiveType::TriangleStripWithAdjacency:    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            case EPrimitiveType::PatchList:                     return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            default:                                            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    constexpr VkPolygonMode ToVkPolygonMode(ERasterFillMode FillMode)
    {
        switch (FillMode)
        {
            case ERasterFillMode::Fill:         return VK_POLYGON_MODE_FILL;
            case ERasterFillMode::Wireframe:    return VK_POLYGON_MODE_LINE;
            default:                            return VK_POLYGON_MODE_FILL;
        }
    }

    constexpr VkCullModeFlags ToVkCullModeFlags(ERasterCullMode CullMode)
    {
        switch (CullMode)
        {
            case ERasterCullMode::Back:     return VK_CULL_MODE_BACK_BIT;
            case ERasterCullMode::Front:    return VK_CULL_MODE_FRONT_BIT;
            case ERasterCullMode::None:     return VK_CULL_MODE_NONE;
            default:                        return VK_CULL_MODE_NONE;
        }
    }

    constexpr VkCompareOp ToVkCompareOp(EComparisonFunc Func)
    {
        switch (Func)
        {
            case EComparisonFunc::Never:            return VK_COMPARE_OP_NEVER;
            case EComparisonFunc::Less:             return VK_COMPARE_OP_LESS;
            case EComparisonFunc::Equal:            return VK_COMPARE_OP_EQUAL;
            case EComparisonFunc::LessOrEqual:      return VK_COMPARE_OP_LESS_OR_EQUAL;
            case EComparisonFunc::Greater:          return VK_COMPARE_OP_GREATER;
            case EComparisonFunc::NotEqual:         return VK_COMPARE_OP_NOT_EQUAL;
            case EComparisonFunc::GreaterOrEqual:   return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case EComparisonFunc::Always:           return VK_COMPARE_OP_ALWAYS;
            default:                                return VK_COMPARE_OP_ALWAYS; 
        }
    }

    constexpr VkStencilOp ToVkStencilOp(EStencilOp Op)
    {
        switch (Op)
        {
            case EStencilOp::Keep:              return VK_STENCIL_OP_KEEP;
            case EStencilOp::Zero:              return VK_STENCIL_OP_ZERO;
            case EStencilOp::Replace:           return VK_STENCIL_OP_REPLACE;
            case EStencilOp::IncrementAndClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            case EStencilOp::DecrementAndClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            case EStencilOp::Invert:            return VK_STENCIL_OP_INVERT;
            case EStencilOp::IncrementAndWrap:  return VK_STENCIL_OP_INCREMENT_AND_WRAP;
            case EStencilOp::DecrementAndWrap:  return VK_STENCIL_OP_DECREMENT_AND_WRAP;
            default:                            return VK_STENCIL_OP_KEEP;
        }
    }

    VkShaderStageFlags ToVkStageFlags(TBitFlags<ERHIShaderType> Type)
    {
        VkShaderStageFlags Flags = 0;
        if (Type.IsFlagSet(ERHIShaderType::Vertex))
        {
            Flags |= VK_SHADER_STAGE_VERTEX_BIT;
        }

        if (Type.IsFlagSet(ERHIShaderType::Fragment))
        {
            Flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        if (Type.IsFlagSet(ERHIShaderType::Compute))
        {
            Flags |= VK_SHADER_STAGE_COMPUTE_BIT;
        }

        return Flags;
    }

    constexpr VkDescriptorType ToVkDescriptorType(ERHIBindingResourceType Type)
    {
        switch (Type)
        {
            case ERHIBindingResourceType::Texture_SRV:  return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case ERHIBindingResourceType::Texture_UAV:  return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case ERHIBindingResourceType::Buffer_SRV:   return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case ERHIBindingResourceType::Buffer_UAV:   return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case ERHIBindingResourceType::Buffer_CBV:   return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            default:                                    Assert(0);
        }
    }


    void ConvertStencilOps(const FDepthStencilState::StencilOpDesc& StencilState, VkPipelineDepthStencilStateCreateInfo& DepthStencilState)
    {
        DepthStencilState.front.failOp = ToVkStencilOp(StencilState.FailOp);
        DepthStencilState.front.depthFailOp = ToVkStencilOp(StencilState.DepthFailOp);
        DepthStencilState.front.passOp = ToVkStencilOp(StencilState.PassOp);
        DepthStencilState.front.compareOp = ToVkCompareOp(StencilState.StencilFunc);
        
        DepthStencilState.back.failOp = ToVkStencilOp(StencilState.FailOp);
        DepthStencilState.back.depthFailOp = ToVkStencilOp(StencilState.DepthFailOp);
        DepthStencilState.back.passOp = ToVkStencilOp(StencilState.PassOp);
        DepthStencilState.back.compareOp = ToVkCompareOp(StencilState.StencilFunc);
    }


    FVulkanBuffer::FVulkanBuffer(FVulkanDevice* InDevice, const FRHIBufferDesc& InDescription)
        : FRHIBuffer(InDescription)
        , IDeviceChild(InDevice)
    {
        VmaAllocationCreateFlags VmaFlags = 0;

        if(GetDescription().Usage.IsFlagSet(EBufferUsageFlags::CPUWritable))
        {
            VmaFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        
        
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = InDescription.Size;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = ToVkBufferUsage(InDescription.Usage);
        BufferCreateInfo.flags = 0;
        
        Device->GetAllocator()->AllocateBuffer(&BufferCreateInfo, VmaFlags, &Buffer, "");
        
        Assert(Buffer != VK_NULL_HANDLE);
    }

    FVulkanBuffer::~FVulkanBuffer()
    {
        Device->GetAllocator()->DestroyBuffer(Buffer);
    }


    //----------------------------------------------------------------------------------------------
    

    FVulkanImage::FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription)
        : FRHIImage(InDescription), IDeviceChild(InDevice)
    {
        VkImageCreateFlags ImageFlags = VK_NO_FLAGS;
        VkImageUsageFlags UsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        
        VkFormat VulkanFormat = GetVkFormat(InDescription.Format);
    
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::RenderTarget))
        {
            UsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::DepthStencil))
        {
            UsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::ShaderResource))
        {
            UsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::Storage))
        {
            UsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::InputAttachment))
        {
            UsageFlags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::UnorderedAccess))
        {
            UsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
    
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::CubeCompatible))
        {
            ImageFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::Aliasable))
        {
            ImageFlags |= VK_IMAGE_CREATE_ALIAS_BIT;
        }
    
        if (InDescription.Flags.IsFlagSet(EImageCreateFlags::DepthStencil))
        {
            FullAspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            PartialAspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (VulkanFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || VulkanFormat == VK_FORMAT_D24_UNORM_S8_UINT)
            {
                FullAspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            FullAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            PartialAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
    
        VkImageCreateInfo ImageCreateInfo = {};
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageCreateInfo.flags = ImageFlags;
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.format = VulkanFormat;
        ImageCreateInfo.extent = { (uint32)GetExtent().X, (uint32)GetExtent().Y, 1 };
        ImageCreateInfo.mipLevels = 1;
        ImageCreateInfo.arrayLayers = 1;
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageCreateInfo.usage = UsageFlags;
        ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
        VmaAllocationCreateFlags AllocationFlags = VK_NO_FLAGS;
        Device->GetAllocator()->AllocateImage(&ImageCreateInfo, AllocationFlags, &Image, "");
    
        Assert(Image != VK_NULL_HANDLE);
    
        VkImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = Image;
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = VulkanFormat;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = FullAspectMask;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
    
        VK_CHECK(vkCreateImageView(Device->GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));
    
        Assert(ImageView != VK_NULL_HANDLE);
    }


    FVulkanImage::FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription, VkImage RawImage, VkImageView RawView)
        : FRHIImage(InDescription)
        , IDeviceChild(InDevice)
        , Image(RawImage)
        , ImageView(RawView)
    {
        bImageManagedExternal = true;
        FullAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        PartialAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    FVulkanImage::~FVulkanImage()
    {
        if (!bImageManagedExternal)
        {
            Device->GetAllocator()->DestroyImage(Image);
        }
        
        vkDestroyImageView(Device->GetDevice(), ImageView, nullptr);
    }

    void* FVulkanImage::GetAPIResourceImpl(EAPIResourceType Type)
    {
        switch (Type)
        {
            case EAPIResourceType::Image:       return Image;
            case EAPIResourceType::ImageView:   return ImageView;
            case EAPIResourceType::Default:     return Image;
            default:                            return Image;
        }

        return nullptr;
    }

    //----------------------------------------------------------------------------------------------


    FVulkanInputLayout::FVulkanInputLayout(const FVertexAttributeDesc* InAttributeDesc, uint32 AttributeCount)
    {

        THashMap<uint32, VkVertexInputBindingDescription> BindingMap;
        uint32 TotalSize = 0;

        for (uint32 i = 0; i < AttributeCount; ++i)
        {
            const FVertexAttributeDesc& Desc = InAttributeDesc[i];

            TotalSize += Desc.ArraySize;
            
            if (BindingMap.find(Desc.BufferIndex) == BindingMap.end())
            {
                VkVertexInputBindingDescription AttributeDescription = {};
                AttributeDescription.binding = Desc.BufferIndex;
                AttributeDescription.stride = Desc.ElementStride;
                AttributeDescription.inputRate = (Desc.bInstanced) ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
                BindingMap[Desc.BufferIndex] = AttributeDescription;
            }
        }

        for (auto& KVP : BindingMap)
        {
            BindingDesc.push_back(KVP.second);
        }

        InputDesc.resize(AttributeCount);
        AttributeDesc.resize(TotalSize);

        uint32 AttributeLocation = 0;
        for (int i = 0; i < AttributeCount; ++i)
        {
            const FVertexAttributeDesc& In = InAttributeDesc[i];
            InputDesc[i] = In;

            uint32 ElementSizeBytes = GetFormatInfo(In.Format).BytesPerBlock;
            uint32 BufferOffset = 0;

            for (uint32 i = 0; i < In.ArraySize; ++i)
            {
                auto& OutAttribute = AttributeDesc[AttributeLocation];
                OutAttribute.location = AttributeLocation;
                OutAttribute.format = ConvertFormat(In.Format);
                OutAttribute.binding = In.BufferIndex;
                OutAttribute.offset= BufferOffset;
                BufferOffset += ElementSizeBytes;

                ++AttributeLocation;
            }
        }
    }

    void* FVulkanInputLayout::GetAPIResourceImpl(EAPIResourceType Type)
    {
        return nullptr;
    }

    uint32 FVulkanInputLayout::GetNumAttributes() const
    {
        return uint32(InputDesc.size());
    }

    const FVertexAttributeDesc* FVulkanInputLayout::GetAttributeDesc(uint32 index) const
    {
        if (index < uint32_t(InputDesc.size()))
        {
            return &InputDesc[index]; 
        }
        else
        {
            return nullptr;
        }
    }

    FVulkanBindingLayout::FVulkanBindingLayout(FVulkanDevice* InDevice, const FBindingLayoutDesc& InDesc)
        :IDeviceChild(InDevice)
    {
        Desc = InDesc;

        for (const FBindingLayoutItem& Item : InDesc.Bindings)
        {
            VkDescriptorSetLayoutBinding Binding = {};
            Binding.descriptorType = ToVkDescriptorType(Item.Type);
            Binding.stageFlags = ToVkStageFlags(InDesc.StageFlags);
            Binding.descriptorCount = 1;
            Binding.binding = Item.Slot;

            Bindings.push_back(Binding);

            VkDescriptorPoolSize PoolSize = {};
            PoolSize.type = Binding.descriptorType;
            PoolSize.descriptorCount = 1;
            PoolSizes.push_back(PoolSize);
        }


        VkDescriptorSetLayoutCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        CreateInfo.pBindings = Bindings.data();
        CreateInfo.bindingCount = Bindings.size();
        CreateInfo.flags = VK_NO_FLAGS;
        
        
        VK_CHECK(vkCreateDescriptorSetLayout(Device->GetDevice(), &CreateInfo, nullptr, &DescriptorSetLayout));
    }

    FVulkanBindingLayout::~FVulkanBindingLayout()
    {
        vkDestroyDescriptorSetLayout(Device->GetDevice(), DescriptorSetLayout, nullptr);
    }

    void* FVulkanBindingLayout::GetAPIResourceImpl(EAPIResourceType Type)
    {
        return DescriptorSetLayout;
    }

    FVulkanBindingSet::FVulkanBindingSet(FVulkanDevice* InDevice, const FBindingSetDesc& InDesc, FVulkanBindingLayout* InLayout)
        : IDeviceChild(InDevice)
        , Desc(InDesc)
        , Layout(InLayout)
    {

        Assert(InLayout->DescriptorSetLayout);
        Assert(!InLayout->PoolSizes.empty());

        VkDescriptorPoolCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        CreateInfo.pPoolSizes = InLayout->PoolSizes.data();
        CreateInfo.poolSizeCount = InLayout->PoolSizes.size();
        CreateInfo.maxSets = 1;
        
        VK_CHECK(vkCreateDescriptorPool(InDevice->GetDevice(), &CreateInfo, nullptr, &DescriptorPool));


        VkDescriptorSetAllocateInfo AllocateInfo = {};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        AllocateInfo.descriptorPool = DescriptorPool;
        AllocateInfo.descriptorSetCount = 1;
        AllocateInfo.pSetLayouts = &InLayout->DescriptorSetLayout;
        
        VK_CHECK(vkAllocateDescriptorSets(InDevice->GetDevice(), &AllocateInfo, &DescriptorSet));


        TVector<VkDescriptorBufferInfo> BufferInfos;
        TVector<VkDescriptorImageInfo> ImageInfos;
        BufferInfos.reserve(InDesc.Bindings.size());
        ImageInfos.reserve(InDesc.Bindings.size());

        
        TVector<VkWriteDescriptorSet> Writes;
        Writes.reserve(4);

        for (SIZE_T BindingIndex = 0; BindingIndex < InDesc.Bindings.size(); ++BindingIndex)
        {
            const FBindingSetItem& Item = InDesc.Bindings[BindingIndex];
            const VkDescriptorSetLayoutBinding VkBinding = Layout->Bindings[BindingIndex];

            
            if ((Item.bUnused) || (Item.ResourceHandle == nullptr))
            {
                continue;
            }

            Resources.push_back(Item.ResourceHandle);

            VkWriteDescriptorSet Write = {};
            Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            Write.descriptorCount = VkBinding.descriptorCount;
            Write.dstArrayElement = 0;
            Write.dstBinding = VkBinding.binding;
            Write.dstSet = DescriptorSet;


            switch (Item.Type)
            {
            case ERHIBindingResourceType::Texture_SRV:
                {
                    BindingsRequiringTransitions.push_back(BindingIndex);
                    
                    FVulkanImage* Image = static_cast<FVulkanImage*>(Item.ResourceHandle);
                    VkDescriptorImageInfo& ImageInfo = ImageInfos.emplace_back();
                    ImageInfo.imageView = Image->GetImageView();
                    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    
                    Write.pImageInfo = &ImageInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                }
                break;
            case ERHIBindingResourceType::Texture_UAV:
                {
                    BindingsRequiringTransitions.push_back(BindingIndex);
                    
                    FVulkanImage* Image = static_cast<FVulkanImage*>(Item.ResourceHandle);
                    VkDescriptorImageInfo& ImageInfo = ImageInfos.emplace_back();
                    ImageInfo.imageView = Image->GetImageView();
                    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    Write.pImageInfo = &ImageInfo;
                }
                break;
            case ERHIBindingResourceType::Buffer_CBV:
            case ERHIBindingResourceType::Buffer_SRV:
                {
                    FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Item.ResourceHandle);
                    VkDescriptorBufferInfo& BufferInfo = BufferInfos.emplace_back();
                    BufferInfo.buffer = Buffer->GetBuffer();
                    BufferInfo.offset = 0;
                    BufferInfo.range = Buffer->GetSize();
                        
                    Write.pBufferInfo = &BufferInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                }
                break;
            case ERHIBindingResourceType::Buffer_UAV:
                {
                    FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Item.ResourceHandle);
                    VkDescriptorBufferInfo& BufferInfo = BufferInfos.emplace_back();
                    BufferInfo.buffer = Buffer->GetBuffer();
                    BufferInfo.offset = 0;
                    BufferInfo.range = Buffer->GetSize();
                        
                    Write.pBufferInfo = &BufferInfo;
                    Write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                }
                break;
            }

            Writes.push_back(Write);
        }

        vkUpdateDescriptorSets(Device->GetDevice(), Writes.size(), Writes.data(), 0, nullptr);
        
    }

    FVulkanBindingSet::~FVulkanBindingSet()
    {
        vkDestroyDescriptorPool(Device->GetDevice(), DescriptorPool, nullptr);
    }

    void* FVulkanBindingSet::GetAPIResourceImpl(EAPIResourceType Type)
    {
        return DescriptorSet;
    }

    FVulkanPipeline::~FVulkanPipeline()
    {
        vkDestroyPipeline(Device->GetDevice(), Pipeline, nullptr);
        vkDestroyPipelineLayout(Device->GetDevice(), PipelineLayout, nullptr);
    }

    void FVulkanPipeline::CreatePipelineLayout(TVector<FRHIBindingLayoutRef> BindingLayouts)
    {
        TVector<VkDescriptorSetLayout> Layouts;
        for (const FRHIBindingLayoutRef& Binding : BindingLayouts)
        {
            FVulkanBindingLayout* VkBindingLayout = Binding.As<FVulkanBindingLayout>();
            Layouts.push_back(VkBindingLayout->DescriptorSetLayout);
        }
        
        VkPipelineLayoutCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        CreateInfo.pSetLayouts = Layouts.data();
        CreateInfo.setLayoutCount = Layouts.size();
        
        VK_CHECK(vkCreatePipelineLayout(Device->GetDevice(), &CreateInfo, nullptr, &PipelineLayout));
    }

    FVulkanGraphicsPipeline::FVulkanGraphicsPipeline(FVulkanDevice* InDevice, const FGraphicsPipelineDesc& InDesc)
        :FVulkanPipeline(InDevice)
    {
        Desc = InDesc;
        
        FVulkanInputLayout* InputLayout = InDesc.InputLayout.As<FVulkanInputLayout>();

        CreatePipelineLayout(InDesc.BindingLayouts);

        
        VkDynamicState DynamicStates[] =
        {
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_VIEWPORT
        };

        
        TVector<VkPipelineShaderStageCreateInfo> ShaderStages;
        VkPipelineShaderStageCreateInfo VertexStage = {};
        VertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        VertexStage.module = InDesc.VS->GetAPIResource<VkShaderModule>();
        VertexStage.pName = "main";
        VertexStage.pNext = nullptr;
        VertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        VertexStage.pSpecializationInfo = nullptr;
        ShaderStages.push_back(VertexStage);

        VkPipelineShaderStageCreateInfo FragmentStage = {};
        FragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        FragmentStage.module = InDesc.PS->GetAPIResource<VkShaderModule>();
        FragmentStage.pName = "main";
        FragmentStage.pNext = nullptr;
        FragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        FragmentStage.pSpecializationInfo = nullptr;
        ShaderStages.push_back(FragmentStage);

        VkPipelineDynamicStateCreateInfo DynamicState = {};
        DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        DynamicState.dynamicStateCount = std::size(DynamicStates);
        DynamicState.pDynamicStates = DynamicStates;
        
        
        VkPipelineVertexInputStateCreateInfo VertexInputState = {};
        VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        VertexInputState.pVertexAttributeDescriptions = InputLayout->AttributeDesc.data();
        VertexInputState.vertexAttributeDescriptionCount = InputLayout->AttributeDesc.size();
        VertexInputState.pVertexBindingDescriptions = InputLayout->BindingDesc.data();
        VertexInputState.vertexBindingDescriptionCount = InputLayout->BindingDesc.size();
        
        
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {};
        InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        InputAssemblyState.topology = ToVkPrimitiveTopology(InDesc.PrimType);
        InputAssemblyState.primitiveRestartEnable = VK_FALSE;
        
        VkPipelineViewportStateCreateInfo ViewportState = {};
        ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ViewportState.viewportCount = 1;
        ViewportState.scissorCount = 1;

        FRasterState RasterState = InDesc.RenderState.RasterState;
        
        VkPipelineRasterizationStateCreateInfo RasterizationState = {};
        RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        RasterizationState.polygonMode = ToVkPolygonMode(RasterState.FillMode);
        RasterizationState.cullMode = ToVkCullModeFlags(RasterState.CullMode);
        RasterizationState.frontFace = RasterState.FrontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
        RasterizationState.depthBiasEnable = RasterState.DepthBias ? VK_TRUE : VK_FALSE;
        RasterizationState.depthBiasConstantFactor = float(RasterState.DepthBias);
        RasterizationState.depthBiasClamp = RasterState.DepthBiasClamp;
        RasterizationState.depthBiasSlopeFactor = RasterState.SlopeScaledDepthBias;
        RasterizationState.lineWidth = 1.0f;
        
        VkPipelineMultisampleStateCreateInfo MultisampleState = {};
        MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        FDepthStencilState DepthState = InDesc.RenderState.DepthStencilState;
        
        VkPipelineDepthStencilStateCreateInfo DepthStencilState = {};
        DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        DepthStencilState.depthTestEnable = DepthState.DepthTestEnable;
        DepthStencilState.depthWriteEnable = DepthState.DepthWriteEnable;
        DepthStencilState.depthCompareOp = ToVkCompareOp(DepthState.DepthFunc);
        DepthStencilState.stencilTestEnable = DepthState.StencilEnable;
        ConvertStencilOps(DepthState.FrontFaceStencil, DepthStencilState);
        ConvertStencilOps(DepthState.BackFaceStencil, DepthStencilState);
        
        VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
        ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        ColorBlendAttachment.blendEnable = VK_FALSE;
        
        VkPipelineColorBlendStateCreateInfo ColorBlendState = {};
        ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        ColorBlendState.attachmentCount = 1;
        ColorBlendState.pAttachments = &ColorBlendAttachment;
        
        VkPipelineRenderingCreateInfo RenderingCreateInfo = {};
        RenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        RenderingCreateInfo.colorAttachmentCount = 1;
        VkFormat ColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
        RenderingCreateInfo.pColorAttachmentFormats = &ColorFormat;
        VkFormat DepthFormat = VK_FORMAT_D32_SFLOAT;
        RenderingCreateInfo.depthAttachmentFormat = DepthFormat;
        RenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
        
        VkGraphicsPipelineCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        CreateInfo.pNext = &RenderingCreateInfo;
        CreateInfo.pVertexInputState = &VertexInputState;
        CreateInfo.pInputAssemblyState = &InputAssemblyState;
        CreateInfo.pViewportState = &ViewportState;
        CreateInfo.pRasterizationState = &RasterizationState;
        CreateInfo.pMultisampleState = &MultisampleState;
        CreateInfo.pDepthStencilState = &DepthStencilState;
        CreateInfo.pColorBlendState = &ColorBlendState;
        CreateInfo.pDynamicState = &DynamicState;
        CreateInfo.stageCount = ShaderStages.size();
        CreateInfo.pStages = ShaderStages.data();
        CreateInfo.layout = PipelineLayout;
        CreateInfo.renderPass = VK_NULL_HANDLE;
        CreateInfo.subpass = 0;
        
        VK_CHECK(vkCreateGraphicsPipelines(Device->GetDevice(), nullptr, 1, &CreateInfo, nullptr, &Pipeline));

    }
    
    FVulkanComputePipeline::FVulkanComputePipeline(FVulkanDevice* InDevice, const FComputePipelineDesc& InDesc)
        :FVulkanPipeline(InDevice)
    {
        Desc = InDesc;

        CreatePipelineLayout(InDesc.BindingLayouts);
        
        VkPipelineShaderStageCreateInfo StageInfo = {};
        StageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        StageInfo.module = InDesc.CS->GetAPIResource<VkShaderModule>();
        StageInfo.pName = "main";
        StageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        
        VkComputePipelineCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        CreateInfo.stage = StageInfo;
        CreateInfo.layout = PipelineLayout;
        
        VK_CHECK(vkCreateComputePipelines(Device->GetDevice(), nullptr, 1, &CreateInfo, nullptr, &Pipeline));
    }
}
