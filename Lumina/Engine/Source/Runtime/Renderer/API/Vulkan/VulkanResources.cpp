#include "VulkanResources.h"

#include "VulkanCommandList.h"
#include "VulkanMacros.h"

namespace Lumina
{

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

    
    FVulkanGraphicsPipeline::FVulkanGraphicsPipeline(const FGraphicsPipelineDesc& InDesc)
    {
        
    }
    
    FVulkanComputePipeline::FVulkanComputePipeline(const FComputePipelineDesc& InDesc)
    {
        
    }
}
