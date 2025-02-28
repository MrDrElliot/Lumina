#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanResources.h"
#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"

namespace Lumina
{
	enum class ERHIPipeline : uint8;
	class FVulkanCommandList;
	enum class ERHIAccess : uint32;
	class FVulkanImage;
}

namespace Lumina
{
	struct FVulkanPipelineBarrier
	{
		FVulkanPipelineBarrier()
			: SrcPipelines()
			, DstPipelines()
			, Semaphore(nullptr)
		{}

		using MemoryBarrierArrayType = TVector<VkMemoryBarrier2>;
		using ImageBarrierArrayType = TVector<VkImageMemoryBarrier2>;
		using BufferBarrierArrayType = TVector<VkBufferMemoryBarrier2>;
	
		ERHIPipeline SrcPipelines, DstPipelines;
		MemoryBarrierArrayType MemoryBarriers;
		ImageBarrierArrayType ImageBarriers;
		BufferBarrierArrayType BufferBarriers;
		VkSemaphore Semaphore;
	
		// We need to keep the texture pointers around, because we need to call OnTransitionResource on them, and we need mip and layer counts for the tracking code.
		struct ImageBarrierExtraData
		{
			FVulkanImage* BaseTexture = nullptr;
			bool IsAliasingBarrier = false;
		};
		
		TVector<ImageBarrierExtraData> ImageBarrierExtras;
	
		void AddMemoryBarrier(VkAccessFlags SrcAccessFlags, VkAccessFlags DstAccessFlags, VkPipelineStageFlags SrcStageMask, VkPipelineStageFlags DstStageMask);
		void AddFullImageLayoutTransition(const FVulkanImage& Texture, VkImageLayout SrcLayout, VkImageLayout DstLayout);
		void AddImageLayoutTransition(VkImage Image, VkImageLayout SrcLayout, VkImageLayout DstLayout, const VkImageSubresourceRange& SubresourceRange) { }
		void AddImageLayoutTransition(VkImage Image, VkImageAspectFlags AspectMask, const struct FVulkanImageLayout& SrcLayout, VkImageLayout DstLayout) { }
		void AddImageLayoutTransition(VkImage Image, VkImageAspectFlags AspectMask, VkImageLayout SrcLayout, const struct FVulkanImageLayout& DstLayout) { }
		void AddImageLayoutTransition(VkImage Image, VkImageAspectFlags AspectMask, const struct FVulkanImageLayout& SrcLayout, const struct FVulkanImageLayout& DstLayout){ }
		void AddImageAccessTransition(const FVulkanImage& Surface, ERHIAccess SrcAccess, ERHIAccess DstAccess, const VkImageSubresourceRange& SubresourceRange, VkImageLayout& InOutLayout) { }
		void Execute(FVulkanCommandList* CmdBuffer);
	
		static VkImageSubresourceRange MakeSubresourceRange(VkImageAspectFlags AspectMask, uint32 FirstMip = 0, uint32 NumMips = VK_REMAINING_MIP_LEVELS, uint32 FirstLayer = 0, uint32 NumLayers = VK_REMAINING_ARRAY_LAYERS);
	};

}
