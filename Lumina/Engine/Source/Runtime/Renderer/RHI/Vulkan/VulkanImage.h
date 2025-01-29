#pragma once
#include "Renderer/Image.h"
#include "vulkanmemoryallocator/src/VmaUsage.h"


namespace Lumina
{
    class FVulkanCommandBuffer;
	constexpr VkFormat convert(const EImageFormat& format)
	{
		switch (format)
		{
			case EImageFormat::R8:							return VK_FORMAT_R8_SRGB;
			case EImageFormat::RB16:						return VK_FORMAT_R8G8_SRGB;
			case EImageFormat::RGB24:						return VK_FORMAT_R8G8B8_SRGB;
			case EImageFormat::RGBA32_SRGB:					return VK_FORMAT_R8G8B8A8_SRGB;
			case EImageFormat::RGBA32_UNORM:				return VK_FORMAT_R8G8B8A8_UNORM;
			case EImageFormat::BGRA32_SRGB:					return VK_FORMAT_B8G8R8A8_SRGB;
			case EImageFormat::BGRA32_UNORM:				return VK_FORMAT_B8G8R8A8_UNORM;
			case EImageFormat::RGB32_HDR:					return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
			case EImageFormat::RGBA64_HDR:					return VK_FORMAT_R16G16B16A16_SFLOAT;
			case EImageFormat::RGBA128_HDR:					return VK_FORMAT_R32G32B32A32_SFLOAT;
			case EImageFormat::D32:							return VK_FORMAT_D32_SFLOAT;
			case EImageFormat::BC7:							return VK_FORMAT_BC7_UNORM_BLOCK;
			case EImageFormat::BC1:							return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case EImageFormat::BC5:							return VK_FORMAT_BC5_UNORM_BLOCK;
			case EImageFormat::BC6h:						return VK_FORMAT_BC6H_UFLOAT_BLOCK;
			case EImageFormat::RGBA64_SFLOAT:				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case EImageFormat::RGB24_UNORM:					return VK_FORMAT_R8G8B8_UNORM;
			default:										std::unreachable();
		}
	}

	constexpr EImageFormat convert(const VkFormat& format)
	{
		switch (format)
		{
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
			case VK_FORMAT_R16G16B16_SFLOAT: 				return EImageFormat::RGBA64_SFLOAT;
			case VK_FORMAT_R8G8B8_UNORM: 					return EImageFormat::RGB24_UNORM;
			default:										std::unreachable();
		}
	}

	constexpr VkImageType convert(const EImageType& type) {
		switch (type)
		{
			case EImageType::TYPE_1D:						return VK_IMAGE_TYPE_1D;
			case EImageType::TYPE_2D:						return VK_IMAGE_TYPE_2D;
			case EImageType::TYPE_3D:						return VK_IMAGE_TYPE_3D;
			default:										std::unreachable();
		}
	}

	constexpr VkImageUsageFlagBits convert(const EImageUsage& usage) {
		switch (usage)
		{
			case EImageUsage::TEXTURE:						return VK_IMAGE_USAGE_SAMPLED_BIT;
			case EImageUsage::RENDER_TARGET:				return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			case EImageUsage::DEPTH_BUFFER:					return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			default:										std::unreachable();
		}
	}

	constexpr VkFilter convert(const ESamplerFilteringMode& mode)
	{
		switch (mode)
		{
			case ESamplerFilteringMode::LINEAR:				return VK_FILTER_LINEAR;
			case ESamplerFilteringMode::NEAREST:			return VK_FILTER_NEAREST;
			default:										std::unreachable();
		}
	}

	constexpr VkSamplerAddressMode convert(const ESamplerAddressMode& mode)
	{
		switch (mode)
		{
			case ESamplerAddressMode::CLAMP:				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
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
			case ESamplerFilteringMode::NEAREST:			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			default:										std::unreachable();
		}
	}
	
    class FVulkanImage : public FImage
    {
    public:

        FVulkanImage(const FImageSpecification& InSpec);
        FVulkanImage(const FImageSpecification& InSpec, VkImage InImage, VkImageView InImageView);

		virtual ~FVulkanImage();
		
		void CreateResolveImage();
        void CreateTexture();
        void CreateRenderTarget();
        void CreateDepthBuffer();
		void CreateFromRaw(const FImageSpecification& InSpec, VkImage InImage, VkImageView InView);
		FImageSpecification GetSpecification() const override { return Spec; }
		
        VkImage GetImage() { return Image; }
		VkImage* GetImagePtr() { return &Image; }
        VkImageView GetImageView() { return ImageView; }
		EImageLayout GetLayout() { return CurrentLayout; }
		
        void SetCurrentLayout(EImageLayout Layout) { CurrentLayout = Layout; }
        void SetLayout(TRefPtr<FCommandBuffer> CmdBuffer, EImageLayout NewLayout, EPipelineStage SrcStage, EPipelineStage DstStage, EPipelineAccess SrcAccess, EPipelineAccess DstAccess) override;

        void SetFriendlyName(const FString& InName) override;
        
    private:

        FImageSpecification Spec;
        bool bCreatedFromRaw;
        VkImage Image = VK_NULL_HANDLE;
        VmaAllocation Allocation = nullptr;
        VkImageView ImageView = VK_NULL_HANDLE;
        EImageLayout CurrentLayout;
    
    };


    class FVulkanImageSampler : public FImageSampler
    {
    public:

        FVulkanImageSampler(const FImageSamplerSpecification& spec);
        void Destroy() override;

    	void SetFriendlyName(const FString& InName) override;
    	
    	VkSampler GetSampler() { return Sampler; }

    private:

    	VkSampler Sampler;
    	FImageSamplerSpecification Specs;
    };
}
