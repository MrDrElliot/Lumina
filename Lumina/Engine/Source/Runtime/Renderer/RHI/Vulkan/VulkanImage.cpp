
#include "VulkanImage.h"
#include "Renderer/RHIIncl.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanMacros.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"

namespace Lumina
{
    FVulkanImage::FVulkanImage(const FImageSpecification& InSpec)
	:Spec(InSpec), CurrentLayout(EImageLayout::UNDEFINED)
    {
    	Guid = FGuid::Generate();
    	bCreatedFromRaw = false;
        switch (InSpec.Usage)
        {
            case(EImageUsage::TEXTURE):         CreateTexture();        break;
            case(EImageUsage::RENDER_TARGET):   CreateRenderTarget();   break;
            case(EImageUsage::DEPTH_BUFFER):    CreateDepthBuffer();    break;
        	case(EImageUsage::RESOLVE):			CreateResolveImage();	break;
        }
    }

    FVulkanImage::FVulkanImage(const FImageSpecification& InSpec, VkImage InImage, VkImageView InImageView)
	:Spec(InSpec), CurrentLayout(EImageLayout::UNDEFINED)
    {
    	Guid = FGuid::Generate();
    	bCreatedFromRaw = true;
    	CreateFromRaw(InSpec, InImage, InImageView);
    }

    FVulkanImage::~FVulkanImage()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    	VkDevice Device = RenderContext->GetDevice();

    	FVulkanMemoryAllocator* Allocator = FVulkanMemoryAllocator::Get();

    	vkDestroyImageView(Device, ImageView, nullptr);
    	Allocator->DestroyImage(Image, Allocation);
    	
    	ImageView = VK_NULL_HANDLE;
    	Image = VK_NULL_HANDLE;
    }

    void FVulkanImage::CreateResolveImage()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

		VkDevice Device = RenderContext->GetDevice();

		VkImageCreateInfo RenderTargetCreateInfo = {};
		RenderTargetCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		RenderTargetCreateInfo.extent = VkExtent3D(Spec.Extent.X, Spec.Extent.Y, 1);
		RenderTargetCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		RenderTargetCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		RenderTargetCreateInfo.mipLevels = 1;
		RenderTargetCreateInfo.arrayLayers = 1;
		RenderTargetCreateInfo.format = convert(Spec.Format);
    	RenderTargetCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // Single-sample resolve image
    	RenderTargetCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; 
		RenderTargetCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		RenderTargetCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		auto Allocator = FVulkanMemoryAllocator::Get();
		Allocation = Allocator->AllocateImage(&RenderTargetCreateInfo, 0, &Image, "RenderTarget");

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

		VK_CHECK(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView));

		TRefCountPtr<FCommandBuffer> TransientCmdBuffer = FCommandBuffer::Create(ECommandBufferLevel::PRIMARY, ECommandBufferType::TRANSIENT, ECommandType::GENERAL);
    	TransientCmdBuffer->SetFriendlyName("Transient Image Command Buffer");

		TransientCmdBuffer->Begin();
    	
		SetLayout(
			TransientCmdBuffer,
			EImageLayout::TRANSFER_DST,
			EPipelineStage::TOP_OF_PIPE,
			EPipelineStage::TRANSFER,
			EPipelineAccess::NONE,
			EPipelineAccess::TRANSFER_WRITE
		);
    	
		TransientCmdBuffer->End();
		TransientCmdBuffer->Execute(true);
    }

    void FVulkanImage::CreateTexture()
    {

    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	
		VkImageCreateInfo texture_create_info = {};
		texture_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		texture_create_info.format = convert(Spec.Format);
		texture_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		texture_create_info.imageType = VK_IMAGE_TYPE_2D;
		texture_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		texture_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		texture_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		texture_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		texture_create_info.arrayLayers = 1;
		texture_create_info.mipLevels = Spec.MipLevels;
		texture_create_info.extent.width = Spec.Extent.X;
    	texture_create_info.extent.height = Spec.Extent.Y;
    	texture_create_info.extent.depth = 1;

		auto allocator = FVulkanMemoryAllocator::Get();
		Allocation = allocator->AllocateImage(&texture_create_info, 0, &Image, "Texture");

		FDeviceBufferSpecification StagingBuffer_spec = {};
		StagingBuffer_spec.Size = Spec.Pixels.size();
		StagingBuffer_spec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
		StagingBuffer_spec.BufferUsage = EDeviceBufferUsage::STAGING_BUFFER;

		TRefCountPtr<FVulkanBuffer> StagingBuffer = MakeRefCount<FVulkanBuffer>(StagingBuffer_spec);
    	StagingBuffer->UploadData(0, Spec.Pixels.data(), Spec.Pixels.size());

		VkDevice device = RenderContext->GetDevice();
		VkCommandBuffer cmd_buffer = RenderContext->AllocateTransientCommandBuffer();

		// So here we need to load and transition layout of all mip-levels of a texture
		// Firstly we transition all of them into transfer destination layout
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = Image;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = texture_create_info.mipLevels;

		vkCmdPipelineBarrier(cmd_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		// Compute transfer regions
		std::vector<VkBufferImageCopy> copy_regions(Spec.MipLevels);
		uint32 buffer_offset = 0;

		for (uint32 i = 0; i < copy_regions.size(); i++)
		{
			glm::uvec2 mip_size = { Spec.Extent.X / (uint32)(std::pow(2, i)), Spec.Extent.Y / (uint32)(std::pow(2, i)) };

			VkBufferImageCopy& buffer_image_copy = copy_regions[i];
			buffer_image_copy.imageExtent = { mip_size.x, mip_size.y, 1 };
			buffer_image_copy.bufferOffset = buffer_offset;
			buffer_image_copy.imageOffset = { 0, 0, 0 };
			buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			buffer_image_copy.imageSubresource.baseArrayLayer = 0;
			buffer_image_copy.imageSubresource.layerCount = 1;
			buffer_image_copy.imageSubresource.mipLevel = i;
			buffer_image_copy.bufferRowLength = 0;
			buffer_image_copy.bufferImageHeight = 0;

			buffer_offset += buffer_image_copy.imageExtent.width * buffer_image_copy.imageExtent.height * 4; //(Spec.Format == EImageFormat::BC7 ? 1 : 4);
		}

		// Submit copy command
		vkCmdCopyBufferToImage(cmd_buffer, StagingBuffer->GetBuffer(), Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32)copy_regions.size(), copy_regions.data());
		
		// Transition layout to shader read only
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = texture_create_info.mipLevels;

		vkCmdPipelineBarrier(cmd_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		// Execute commands
		RenderContext->ExecuteTransientCommandBuffer(cmd_buffer);

		CurrentLayout = EImageLayout::SHADER_READ_ONLY;

		// Clear image data
		//Spec.Pixels.clear();

		// Create image view
		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.format = convert(Spec.Format);
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = texture_create_info.mipLevels;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		VK_CHECK(vkCreateImageView(device, &ImageViewCreateInfo, nullptr, &ImageView));
    	
	}

    void FVulkanImage::CreateRenderTarget()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	VkDevice Device = RenderContext->GetDevice();

		VkImageCreateInfo RenderTargetCreateInfo = {};
		RenderTargetCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		RenderTargetCreateInfo.extent = VkExtent3D(Spec.Extent.X, Spec.Extent.Y, 1);
		RenderTargetCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		RenderTargetCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		RenderTargetCreateInfo.mipLevels = 1;
		RenderTargetCreateInfo.arrayLayers = 1;
		RenderTargetCreateInfo.format = convert(Spec.Format);
		RenderTargetCreateInfo.samples = Spec.SampleCount == EImageSampleCount::ONE ? VK_SAMPLE_COUNT_1_BIT : VK_SAMPLE_COUNT_4_BIT;
		RenderTargetCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; 
		RenderTargetCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		RenderTargetCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		auto Allocator = FVulkanMemoryAllocator::Get();
		Allocation = Allocator->AllocateImage(&RenderTargetCreateInfo, 0, &Image, "RenderTarget");

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

		VK_CHECK(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView));

		FRHICommandBuffer TransientCmdBuffer = FCommandBuffer::Create(ECommandBufferLevel::PRIMARY, ECommandBufferType::TRANSIENT, ECommandType::GENERAL);
    	TransientCmdBuffer->SetFriendlyName("Transient Render Target Command Buffer");

		TransientCmdBuffer->Begin();
    	
		SetLayout(TransientCmdBuffer, EImageLayout::COLOR_ATTACHMENT, EPipelineStage::TOP_OF_PIPE, EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
			EPipelineAccess::NONE,
			EPipelineAccess::COLOR_ATTACHMENT_WRITE
		);
    	
		TransientCmdBuffer->End();
		TransientCmdBuffer->Execute(true);
    }

    void FVulkanImage::CreateDepthBuffer()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	VkDevice Device = RenderContext->GetDevice();

		VkImageCreateInfo DepthBufferCreateInfo = {};
		DepthBufferCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		DepthBufferCreateInfo.extent = VkExtent3D(Spec.Extent.X, Spec.Extent.Y, 1);
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
		Allocation = Allocator->AllocateImage(&DepthBufferCreateInfo, 0, &Image, "Depth Buffer");

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

		VK_CHECK(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &ImageView));

    	FRHICommandBuffer TransientCmdBuffer = FCommandBuffer::Create(ECommandBufferLevel::PRIMARY, ECommandBufferType::TRANSIENT, ECommandType::GENERAL);
    	TransientCmdBuffer->SetFriendlyName("Transient Depth Buffer Command Buffer");

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
    }

    void FVulkanImage::CreateFromRaw(const FImageSpecification& InSpec, VkImage InImage, VkImageView InView)
    {
    	Image = InImage;
    	ImageView = InView;
    	Spec = InSpec;
    }
	
    void FVulkanImage::SetLayout(FRHICommandBuffer CmdBuffer, EImageLayout NewLayout, EPipelineStage SrcStage, EPipelineStage DstStage, EPipelineAccess SrcAccess, EPipelineAccess DstAccess)
    {
    	TRefCountPtr<FVulkanCommandBuffer> CommandBuffer = CmdBuffer.As<FVulkanCommandBuffer>();
    	Assert(CommandBuffer != nullptr);

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

    void FVulkanImage::SetFriendlyName(const FString& InName)
    {
	    FImage::SetFriendlyName(InName);

    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	VkDevice Device = RenderContext->GetDevice();
        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = GetFriendlyName().c_str();
    	NameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
    	NameInfo.objectHandle = reinterpret_cast<uint64_t>(Image);
    	
    	RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);

		NameInfo.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
    	NameInfo.objectHandle = reinterpret_cast<uint64_t>(ImageView);
    	
    	RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);

    }


    FVulkanImageSampler::FVulkanImageSampler(const FImageSamplerSpecification& spec)
    {
    	Specs = spec;
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	VkDevice Device = RenderContext->GetDevice();

    	VkSamplerCreateInfo SamplerCreateInfo = {};
    	SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    	SamplerCreateInfo.minFilter = convert(spec.MinFilteringMode);
    	SamplerCreateInfo.magFilter = convert(spec.MagFilteringMode);
    	SamplerCreateInfo.mipmapMode = convertMipmapMode(spec.MipMapFilteringMode);
    	SamplerCreateInfo.addressModeU = convert(spec.AddressMode);
    	SamplerCreateInfo.addressModeV = convert(spec.AddressMode);
    	SamplerCreateInfo.addressModeW = convert(spec.AddressMode);
    	SamplerCreateInfo.anisotropyEnable = spec.AnisotropicFilteringLevel == 1 ? VK_FALSE : VK_TRUE;
    	SamplerCreateInfo.maxAnisotropy = (glm::float32)spec.AnisotropicFilteringLevel;
    	SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    	SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    	SamplerCreateInfo.compareEnable = VK_FALSE;
    	SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    	SamplerCreateInfo.mipLodBias = spec.LODBias;
    	SamplerCreateInfo.minLod = spec.MinLOD;
    	SamplerCreateInfo.maxLod = spec.MaxLOD;

    	VK_CHECK(vkCreateSampler(Device, &SamplerCreateInfo, nullptr, &Sampler));
    	
    }

    void FVulkanImageSampler::Destroy()
    {
    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	VkDevice Device = RenderContext->GetDevice();
    	vkDestroySampler(Device, Sampler, nullptr);
    	Sampler = VK_NULL_HANDLE;
    }

    void FVulkanImageSampler::SetFriendlyName(const FString& InName)
    {
	    FImageSampler::SetFriendlyName(InName);

    	FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

    	
    	VkDevice Device = RenderContext->GetDevice();
        
    	VkDebugUtilsObjectNameInfoEXT NameInfo = {};
    	NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    	NameInfo.pObjectName = GetFriendlyName().c_str();
    	NameInfo.objectType = VK_OBJECT_TYPE_SAMPLER;
    	NameInfo.objectHandle = reinterpret_cast<uint64_t>(Sampler);
    	
    	RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
    	
    }
	
}
