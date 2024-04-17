#include "VulkanImage.h"

#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"
#include "stb_image/stb_image.h"

namespace Lumina
{
    FVulkanImage::FVulkanImage(const FImageSpecification& InSpec)
	    :CurrentLayout(EImageLayout::UNDEFINED)
    {
        switch (InSpec.Usage)
        {
            case(EImageUsage::TEXTURE):         CreateTexture();        break;
            case(EImageUsage::RENDER_TARGET):   CreateRenderTarget();   break;
            case(EImageUsage::DEPTH_BUFFER):    CreateDepthBuffer();    break;
        }
    }

    FVulkanImage::FVulkanImage(const FImageSpecification& InSpec, VkImage InImage, VkImageView InImageView)
	:CurrentLayout(EImageLayout::UNDEFINED)
    {
    	bCreatedFromRaw = true;
    	CreateFromRaw(InSpec, InImage, InImageView);
    }

    void FVulkanImage::CreateTexture()
    {
    	/*stbi_set_flip_vertically_on_load(true);

		int ImageWidth, ImageHeight, ChannelCount;
		glm::uint8* ImageData = stbi_load(Spec.Path.string().c_str(), &ImageWidth, &ImageHeight, &ChannelCount, STBI_rgb_alpha);
		Spec.Extent = { (glm::uint32)ImageWidth, (glm::uint32)ImageHeight, 0 };

		VkImageCreateInfo TextureCreateInfo = {};
		TextureCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		TextureCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		TextureCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		TextureCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		TextureCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		TextureCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		TextureCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		TextureCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		TextureCreateInfo.arrayLayers = 1;
		TextureCreateInfo.mipLevels = log2((std::max)(ImageWidth, ImageHeight)) + 1;
		TextureCreateInfo.extent = { (glm::uint32)ImageWidth, (glm::uint32)ImageHeight, 1 };

		auto Allocator = FVulkanMemoryAllocator::Get();
		Allocation = Allocator->AllocateImage(&TextureCreateInfo, 0, &Image);

		FDeviceBufferSpecification StagingBufferSpec = {};
		StagingBufferSpec.Size = ImageWidth * ImageHeight * STBI_rgb_alpha;
		StagingBufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
		StagingBufferSpec.BufferUsage = EDeviceBufferUsage::STAGING_BUFFER;

		FVulkanBuffer StagingBuffer(StagingBufferSpec, ImageData, ImageWidth * ImageHeight * STBI_rgb_alpha);
		
		VkBufferImageCopy BufferImageCopy = {};
		BufferImageCopy.imageExtent = { (glm::uint32)ImageWidth, (glm::uint32)ImageHeight, 1 };
		BufferImageCopy.bufferOffset = 0;
		BufferImageCopy.imageOffset = { 0, 0, 0 };
		BufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		BufferImageCopy.imageSubresource.baseArrayLayer = 0;
		BufferImageCopy.imageSubresource.layerCount = 1;
		BufferImageCopy.imageSubresource.mipLevel = 0;
		BufferImageCopy.bufferRowLength = 0;
		BufferImageCopy.bufferImageHeight = 0;

		auto Device = FVulkanRenderContext::Get().GetDevice();
		VkCommandBuffer CmdBuffer = FVulkanRenderContext::AllocateTransientCommandBuffer();
    	
		VkImageMemoryBarrier Barrier = {};
		Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		Barrier.image = Image;
		Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		Barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Barrier.subresourceRange.baseArrayLayer = 0;
		Barrier.subresourceRange.layerCount = 1;
		Barrier.subresourceRange.baseMipLevel = 0;
		Barrier.subresourceRange.levelCount = TextureCreateInfo.mipLevels;

		vkCmdPipelineBarrier(CmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &Barrier);

		vkCmdCopyBufferToImage(CmdBuffer, StagingBuffer.GetBuffer(), Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy);
		for (int i = 0; i < TextureCreateInfo.mipLevels - 1; i++) 
		{
			// Transition previous mip to transfer source layout
			Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			Barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			Barrier.subresourceRange.levelCount = 1;
			Barrier.subresourceRange.baseMipLevel = i;

			vkCmdPipelineBarrier(CmdBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&Barrier
			);

			if (i >= TextureCreateInfo.mipLevels) break;
			
			VkImageBlit image_blit_params = {};
			image_blit_params.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_blit_params.srcSubresource.baseArrayLayer = 0;
			image_blit_params.srcSubresource.layerCount = 1;
			image_blit_params.srcSubresource.mipLevel = i;
			image_blit_params.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_blit_params.dstSubresource.baseArrayLayer = 0;
			image_blit_params.dstSubresource.layerCount = 1;
			image_blit_params.dstSubresource.mipLevel = i + 1;
			image_blit_params.srcOffsets[0] = { 0, 0, 0 };
			image_blit_params.srcOffsets[1] = { ImageWidth, ImageHeight, 1 };
			image_blit_params.dstOffsets[0] = { 0, 0, 0 };
			image_blit_params.dstOffsets[1] = { ImageWidth / 2, ImageHeight / 2, 1 };

			vkCmdBlitImage(CmdBuffer, Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_blit_params, VK_FILTER_LINEAR);

			ImageWidth /= 2;
			ImageHeight /= 2;
		}

		Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.subresourceRange.baseMipLevel = 0;
		Barrier.subresourceRange.levelCount = TextureCreateInfo.mipLevels - 1;

		// last image is TRANSFER_DST_OPTIMAL, so need to transition it separately
		VkImageMemoryBarrier LastMipLevelBarrier = {};
		LastMipLevelBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		LastMipLevelBarrier.image = Image;
		LastMipLevelBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		LastMipLevelBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		LastMipLevelBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		LastMipLevelBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		LastMipLevelBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		LastMipLevelBarrier.subresourceRange.baseArrayLayer = 0;
		LastMipLevelBarrier.subresourceRange.layerCount = 1;
		LastMipLevelBarrier.subresourceRange.baseMipLevel = TextureCreateInfo.mipLevels - 1;
		LastMipLevelBarrier.subresourceRange.levelCount = 1;

		VkImageMemoryBarrier final_barriers[2] = { Barrier, LastMipLevelBarrier };

		vkCmdPipelineBarrier(CmdBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			2,
			final_barriers
		);

		FVulkanRenderContext::ExecuteTransientCommandBuffer(CmdBuffer);

		CurrentLayout = EImageLayout::SHADER_READ_ONLY;

		stbi_image_free(ImageData);

		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = TextureCreateInfo.mipLevels;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView);

		Spec.ArrayLayers = 1;
		Spec.MipLevels = TextureCreateInfo.mipLevels;
		Spec.Type = EImageType::TYPE_2D;
		Spec.Usage = EImageUsage::TEXTURE;
		Spec.Format = EImageFormat::RGBA32_UNORM;

		StagingBuffer.Destroy();*/
    }

    void FVulkanImage::CreateRenderTarget()
    {
    	
    }

    void FVulkanImage::CreateDepthBuffer()
    {
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
    	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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
    }

    void FVulkanImageSampler::Destroy()
    {
    }
}
