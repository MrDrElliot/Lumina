#include "VulkanImage.h"

#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"
#include "stb_image/stb_image.h"

namespace Lumina
{
    FVulkanImage::FVulkanImage(const FImageSpecification& InSpec)
	:Spec(InSpec), CurrentLayout(EImageLayout::UNDEFINED)
    {
    	bCreatedFromRaw = false;
        switch (InSpec.Usage)
        {
            case(EImageUsage::TEXTURE):         CreateTexture();        break;
            case(EImageUsage::RENDER_TARGET):   CreateRenderTarget();   break;
            case(EImageUsage::DEPTH_BUFFER):    CreateDepthBuffer();    break;
        }
    }

    FVulkanImage::FVulkanImage(const FImageSpecification& InSpec, VkImage InImage, VkImageView InImageView)
	:Spec(InSpec), CurrentLayout(EImageLayout::UNDEFINED)
    {
    	bCreatedFromRaw = true;
    	CreateFromRaw(InSpec, InImage, InImageView);
    }

    void FVulkanImage::CreateTexture()
    {
 
    }

    void FVulkanImage::CreateRenderTarget()
    {
    	auto Device = FVulkanRenderContext::GetDevice();

		VkImageCreateInfo RenderTargetCreateInfo = {};
		RenderTargetCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		RenderTargetCreateInfo.extent = { Spec.Extent.x, Spec.Extent.y, 1 };
		RenderTargetCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		RenderTargetCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		RenderTargetCreateInfo.mipLevels = 1;
		RenderTargetCreateInfo.arrayLayers = 1;
		RenderTargetCreateInfo.format = convert(Spec.Format);
		RenderTargetCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // HACK
		RenderTargetCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		RenderTargetCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		RenderTargetCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		auto Allocator = FVulkanMemoryAllocator::Get();
		Allocation = Allocator->AllocateImage(&RenderTargetCreateInfo, 0, &Image);

		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.format = convert(Spec.Format);
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView);

		std::shared_ptr<FCommandBuffer> TransientCmdBuffer = FCommandBuffer::Create(ECommandBufferLevel::PRIMARY,
			ECommandBufferType::TRANSIENT, ECommandType::GENERAL);

		TransientCmdBuffer->Begin();
    	
		SetLayout(
			TransientCmdBuffer,
			EImageLayout::COLOR_ATTACHMENT,
			EPipelineStage::TOP_OF_PIPE,
			EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
			EPipelineAccess::NONE,
			EPipelineAccess::COLOR_ATTACHMENT_WRITE
		);
		TransientCmdBuffer->End();
		TransientCmdBuffer->Execute(true);
		TransientCmdBuffer->Destroy();
    }

    void FVulkanImage::CreateDepthBuffer()
    {
    	auto Device = FVulkanRenderContext::GetDevice();

		VkImageCreateInfo DepthBufferCreateInfo = {};
		DepthBufferCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		DepthBufferCreateInfo.extent = { Spec.Extent.x, Spec.Extent.y, 1 };
		DepthBufferCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		DepthBufferCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthBufferCreateInfo.mipLevels = 1;
		DepthBufferCreateInfo.arrayLayers = 1;
		DepthBufferCreateInfo.format = convert(Spec.Format);
		DepthBufferCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // HACK
		DepthBufferCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		DepthBufferCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		DepthBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto Allocator = FVulkanMemoryAllocator::Get();
		Allocation = Allocator->AllocateImage(&DepthBufferCreateInfo, 0, &Image);

		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.format = convert(Spec.Format);
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView);

    	std::shared_ptr<FCommandBuffer> TransientCmdBuffer = FCommandBuffer::Create(ECommandBufferLevel::PRIMARY,
    		ECommandBufferType::TRANSIENT, ECommandType::GENERAL);

		TransientCmdBuffer->Begin();
    	
		SetLayout(
			TransientCmdBuffer,
			EImageLayout::DEPTH_STENCIL_ATTACHMENT,
			EPipelineStage::TOP_OF_PIPE,
			EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
			EPipelineAccess::NONE,
			EPipelineAccess::COLOR_ATTACHMENT_WRITE
		);
    	
		TransientCmdBuffer->End();
		TransientCmdBuffer->Execute(true);
		TransientCmdBuffer->Destroy();
    }

    void FVulkanImage::CreateFromRaw(const FImageSpecification& InSpec, VkImage InImage, VkImageView InView)
    {
    	Image = InImage;
    	ImageView = InView;
    	Spec = InSpec;
    }

    void FVulkanImage::Destroy()
    {
        auto Device = FVulkanRenderContext::GetDevice();
        auto Allocator = FVulkanMemoryAllocator::Get();

        vkDestroyImageView(Device, ImageView, nullptr);
        Allocator->DestroyImage(&Image, &Allocation);
		

        ImageView = VK_NULL_HANDLE;
    }

    void FVulkanImage::SetLayout(std::shared_ptr<FCommandBuffer> CmdBuffer, EImageLayout NewLayout,
	    EPipelineStage SrcStage, EPipelineStage DstStage, EPipelineAccess SrcAccess, EPipelineAccess DstAccess)
    {
    	std::shared_ptr<FVulkanCommandBuffer> CommandBuffer = std::dynamic_pointer_cast<FVulkanCommandBuffer>(CmdBuffer);

    	VkImageMemoryBarrier ImageMemoryBarrier = {};
    	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    	ImageMemoryBarrier.image = Image;
    	ImageMemoryBarrier.oldLayout = (VkImageLayout)CurrentLayout;
    	ImageMemoryBarrier.newLayout = (VkImageLayout)NewLayout;
    	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    	ImageMemoryBarrier.srcAccessMask = (VkAccessFlags)SrcAccess;
    	ImageMemoryBarrier.dstAccessMask = (VkAccessFlags)DstAccess;
    	ImageMemoryBarrier.subresourceRange.aspectMask = Spec.Usage == EImageUsage::DEPTH_BUFFER ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    	ImageMemoryBarrier.subresourceRange.layerCount = 1;
    	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    	ImageMemoryBarrier.subresourceRange.levelCount = 1;

    	vkCmdPipelineBarrier(CommandBuffer->GetCommandBuffer(),
			(VkPipelineStageFlags)SrcStage,
			(VkPipelineStageFlags)DstStage,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&ImageMemoryBarrier
		);

    	CurrentLayout = NewLayout;
    }


    FVulkanImageSampler::FVulkanImageSampler(const FImageSamplerSpecification& spec)
    {
    	Specs = spec;
    	auto Device = FVulkanRenderContext::GetDevice();

    	VkSamplerCreateInfo SamplerCreateInfo = {};
    	SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    	SamplerCreateInfo.minFilter = convert(spec.MinFilteringMode);
    	SamplerCreateInfo.magFilter = convert(spec.MagFilteringMode);
    	SamplerCreateInfo.mipmapMode = convertMipmapMode(spec.MipMapFilteringMode);
    	SamplerCreateInfo.addressModeU = convert(spec.AddressMode);
    	SamplerCreateInfo.addressModeV = convert(spec.AddressMode);
    	SamplerCreateInfo.addressModeW = convert(spec.AddressMode);
    	SamplerCreateInfo.anisotropyEnable = spec.AnisotropicFilteringLevel == 1.0f ? VK_FALSE : VK_TRUE;
    	SamplerCreateInfo.maxAnisotropy = (glm::float32)spec.AnisotropicFilteringLevel;
    	SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    	SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    	SamplerCreateInfo.compareEnable = VK_FALSE;
    	SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    	SamplerCreateInfo.mipLodBias = spec.LODBias;
    	SamplerCreateInfo.minLod = spec.MinLOD;
    	SamplerCreateInfo.maxLod = spec.MaxLOD;

    	vkCreateSampler(Device, &SamplerCreateInfo, nullptr, &Sampler);
    	
    }

    void FVulkanImageSampler::Destroy()
    {
    	auto Device = FVulkanRenderContext::GetDevice();
    	vkDestroySampler(Device, Sampler, nullptr);
    	Sampler = VK_NULL_HANDLE;
    }
}
