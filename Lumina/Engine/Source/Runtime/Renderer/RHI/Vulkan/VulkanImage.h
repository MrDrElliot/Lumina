#pragma once
#include "Source/Runtime/Renderer/Image.h"
#include "Source/Runtime/Renderer/PipelineStage.h"
#include "vulkanmemoryallocator/src/VmaUsage.h"


namespace Lumina
{
    class FVulkanCommandBuffer;

    #pragma region converts
	constexpr VkFormat convert(const EImageFormat& format)
	{
		switch (format)
		{
		case EImageFormat::R8:							return VK_FORMAT_R8_SRGB;
		case EImageFormat::RB16:							return VK_FORMAT_R8G8_SRGB;
		case EImageFormat::RGB24:						return VK_FORMAT_R8G8B8_SRGB;
		case EImageFormat::RGBA32_SRGB:					return VK_FORMAT_R8G8B8A8_SRGB;
		case EImageFormat::RGBA32_UNORM:					return VK_FORMAT_R8G8B8A8_UNORM;
		case EImageFormat::BGRA32_SRGB:					return VK_FORMAT_B8G8R8A8_SRGB;
		case EImageFormat::BGRA32_UNORM:					return VK_FORMAT_B8G8R8A8_UNORM;
		case EImageFormat::RGB32_HDR:					return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case EImageFormat::RGBA64_HDR:					return VK_FORMAT_R16G16B16A16_SFLOAT;
		case EImageFormat::RGBA128_HDR:					return VK_FORMAT_R32G32B32A32_SFLOAT;
		case EImageFormat::D32:							return VK_FORMAT_D32_SFLOAT;
		default:
			std::unreachable();
			break;
		}
	}

	constexpr EImageFormat convert(const VkFormat& format)
	{
		switch (format) {
		case VK_FORMAT_R8_SRGB:							return EImageFormat::R8;
		case VK_FORMAT_R8G8_SRGB:						return EImageFormat::RB16;
		case VK_FORMAT_R8G8B8_SRGB:						return EImageFormat::RGB24;
		case VK_FORMAT_R8G8B8A8_SRGB:					return EImageFormat::RGBA32_SRGB;
		case VK_FORMAT_B8G8R8A8_SRGB:					return EImageFormat::BGRA32_SRGB;
		case VK_FORMAT_B8G8R8A8_UNORM:					return EImageFormat::BGRA32_UNORM;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:			return EImageFormat::RGB32_HDR;
		case VK_FORMAT_R16G16B16A16_SFLOAT:				return EImageFormat::RGBA64_HDR;
		case VK_FORMAT_R32G32B32A32_SFLOAT:				return EImageFormat::RGBA128_HDR;
		case VK_FORMAT_D32_SFLOAT:						return EImageFormat::D32;
		default:										std::unreachable(); break;;
		}
	}

	constexpr VkImageType convert(const EImageType& type) {
		switch (type)
		{
		case EImageType::TYPE_1D:						return VK_IMAGE_TYPE_1D;
		case EImageType::TYPE_2D:						return VK_IMAGE_TYPE_2D;
		case EImageType::TYPE_3D:						return VK_IMAGE_TYPE_3D;
		default:										std::unreachable(); break;
		}
	}

	constexpr VkImageUsageFlagBits convert(const EImageUsage& usage) {
		switch (usage)
		{
		case EImageUsage::TEXTURE:						return VK_IMAGE_USAGE_SAMPLED_BIT;
		case EImageUsage::RENDER_TARGET:					return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		case EImageUsage::DEPTH_BUFFER:					return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		default:										std::unreachable(); break;
		}
	}

	constexpr VkFilter convert(const ESamplerFilteringMode& mode)
	{
		switch (mode)
		{
		case ESamplerFilteringMode::LINEAR:				return VK_FILTER_LINEAR;
		case ESamplerFilteringMode::NEAREST:				return VK_FILTER_NEAREST;
		default:										std::unreachable();
		}
	}

	constexpr VkSamplerAddressMode convert(const ESamplerAddressMode& mode)
	{
		switch (mode)
		{
		case ESamplerAddressMode::CLAMP:					return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case ESamplerAddressMode::CLAMP_BORDER:			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case ESamplerAddressMode::REPEAT:				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case ESamplerAddressMode::MIRRORED_REPEAT:		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		default:										std::unreachable();
		}
	}

	constexpr VkSamplerMipmapMode convertMipmapMode(const ESamplerFilteringMode& mode)
	{
		switch (mode)
		{
		case ESamplerFilteringMode::LINEAR:				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case ESamplerFilteringMode::NEAREST:				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		default:										std::unreachable();
		}
	}

#pragma endregion
    
    class FVulkanImage : public FImage
    {
    public:

        FVulkanImage(const FImageSpecification& InSpec);
        FVulkanImage(const FImageSpecification& InSpec, VkImage InImage, VkImageView InImageView);

        void CreateTexture();
        void CreateRenderTarget();
        void CreateDepthBuffer();
		void CreateFromRaw(const FImageSpecification& InSpec, VkImage InImage, VkImageView InView);


        void Destroy() override;

        VkImage GetImage() { return Image; }
        VkImageView GetImageView() { return ImageView; }
            
        void SetCurrentLayout(EImageLayout Layout) { CurrentLayout = Layout; }
        void SetLayout(std::shared_ptr<FCommandBuffer> CmdBuffer, EImageLayout NewLayout, EPipelineStage SrcStage, EPipelineStage DstStage, EPipelineAccess SrcAccess, EPipelineAccess DstAccess) override;

        
        
    private:

        FImageSpecification Spec;
        bool bCreatedFromRaw;
        VkImage Image;
        VmaAllocation Allocation;
        VkImageView ImageView;
        EImageLayout CurrentLayout;
    
    };


    class FVulkanImageSampler : public FImageSampler
    {
    public:

        FVulkanImageSampler(const FImageSamplerSpecification& spec);

        void Destroy() override;
    };
}
