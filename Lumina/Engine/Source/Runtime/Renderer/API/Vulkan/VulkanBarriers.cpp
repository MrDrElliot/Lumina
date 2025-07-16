#include "VulkanBarriers.h"

#include "VulkanCommandList.h"
#include "VulkanRenderContext.h"


namespace Lumina
{

	static VkAccessFlags GetVkAccessMaskForLayout(VkImageLayout layout)
	{
	    switch (layout)
	    {
	        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
	            return VK_ACCESS_TRANSFER_READ_BIT;
	
	        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
	            return VK_ACCESS_TRANSFER_WRITE_BIT;
	
	        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
	            return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
	        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
	        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
	        case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
	            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	
	        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
	        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
	        case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
	            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	
	        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
	            return VK_ACCESS_SHADER_READ_BIT;
	
	        case VK_IMAGE_LAYOUT_GENERAL:
	            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	
	        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
	            return VK_ACCESS_MEMORY_READ_BIT;
	
	        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
	            return VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
	
	        case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
	            return VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
	
	        case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
	            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	
	        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
	        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
	            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	
	        case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
	            return VK_ACCESS_SHADER_READ_BIT;
	
	        case VK_IMAGE_LAYOUT_UNDEFINED:
	        case VK_IMAGE_LAYOUT_PREINITIALIZED:
	        default:
	            return 0;
	    }
	}
		
    static VkPipelineStageFlags GetVkStageFlagsForLayout(VkImageLayout Layout)
	{
		VkPipelineStageFlags Flags = 0;
	
		switch (Layout)
		{
			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
			case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
			case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
			case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
			case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				Flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
				Flags = VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
				break;
	
			case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
				Flags = VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
				break;
				
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_UNDEFINED:
				Flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				break;
	
			case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
				Flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				break;
	
			default:
				break;
		}
	
		return Flags;
	}
    
    // Helpers for filling in the fields of a VkImageMemoryBarrier structure.
    static void SetupImageBarrier(VkImageMemoryBarrier2& ImgBarrier, VkImage Image, VkPipelineStageFlags SrcStageFlags, VkPipelineStageFlags DstStageFlags, 
    VkAccessFlags SrcAccessFlags, VkAccessFlags DstAccessFlags, VkImageLayout SrcLayout, VkImageLayout DstLayout, const VkImageSubresourceRange& SubresRange)
    {
        ImgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        ImgBarrier.pNext = nullptr;
        ImgBarrier.srcStageMask = SrcStageFlags;
        ImgBarrier.dstStageMask = DstStageFlags;
        ImgBarrier.srcAccessMask = SrcAccessFlags;
        ImgBarrier.dstAccessMask = DstAccessFlags;
        ImgBarrier.oldLayout = SrcLayout;
        ImgBarrier.newLayout = DstLayout;
        ImgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ImgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ImgBarrier.image = Image;
        ImgBarrier.subresourceRange = SubresRange;
    }
    
    void FVulkanPipelineBarrier::AddMemoryBarrier(VkAccessFlags SrcAccessFlags, VkAccessFlags DstAccessFlags, VkPipelineStageFlags SrcStageMask, VkPipelineStageFlags DstStageMask)
    {
	    constexpr VkAccessFlags ReadMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT |
        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT;

        if (MemoryBarriers.empty())
        {
            VkMemoryBarrier2* NewBarrier = (VkMemoryBarrier2*)MemoryBarriers.push_back_uninitialized();
            NewBarrier->sType =  VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
        }
        

        // Mash everything into a single barrier
        VkMemoryBarrier2& MemoryBarrier = MemoryBarriers[0];

        // We only need a memory barrier if the previous commands wrote to the buffer. In case of a transition from read, an execution barrier is enough.
        const bool SrcAccessIsRead = ((SrcAccessFlags & (~ReadMask)) == 0);
        if (!SrcAccessIsRead)
        {
            MemoryBarrier.srcAccessMask |= SrcAccessFlags;
            MemoryBarrier.dstAccessMask |= DstAccessFlags;
        }

        MemoryBarrier.srcStageMask |= SrcStageMask;
        MemoryBarrier.dstStageMask |= DstStageMask;
    }

    void FVulkanPipelineBarrier::AddFullImageLayoutTransition(const FVulkanImage& Texture, VkImageLayout SrcLayout, VkImageLayout DstLayout)
    {
        const VkPipelineStageFlags SrcStageMask = GetVkStageFlagsForLayout(SrcLayout);
        const VkPipelineStageFlags DstStageMask = GetVkStageFlagsForLayout(DstLayout);

        const VkAccessFlags SrcAccessFlags = GetVkAccessMaskForLayout(SrcLayout);
        const VkAccessFlags DstAccessFlags = GetVkAccessMaskForLayout(DstLayout);

        const VkImageSubresourceRange SubresourceRange = MakeSubresourceRange(Texture.GetFullAspectMask());

        VkImageMemoryBarrier2 ImgBarrier;
        SetupImageBarrier(ImgBarrier, Texture.GetImage(), SrcStageMask, DstStageMask, SrcAccessFlags, DstAccessFlags, SrcLayout, DstLayout, SubresourceRange);
		ImageBarriers.push_back(Memory::Move(ImgBarrier));
    }
	

	void FVulkanPipelineBarrier::Execute(FVulkanCommandList* CmdBuffer)
	{
		if (!MemoryBarriers.empty())
		{
			VkDependencyInfo depInfo = {};
			depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			depInfo.memoryBarrierCount = static_cast<uint32>(MemoryBarriers.size());
			depInfo.pMemoryBarriers = MemoryBarriers.data();

			vkCmdPipelineBarrier2(CmdBuffer->CurrentCommandBuffer->CommandBuffer, &depInfo);
		}

		if (!ImageBarriers.empty())
		{
			VkDependencyInfo depInfo = {};
			depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			depInfo.imageMemoryBarrierCount = static_cast<uint32>(ImageBarriers.size());
			depInfo.pImageMemoryBarriers = ImageBarriers.data();

			vkCmdPipelineBarrier2(CmdBuffer->CurrentCommandBuffer->CommandBuffer, &depInfo);
		}

		if (!BufferBarriers.empty())
		{
			VkDependencyInfo depInfo = {};
			depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			depInfo.bufferMemoryBarrierCount = static_cast<uint32>(BufferBarriers.size());
			depInfo.pBufferMemoryBarriers = BufferBarriers.data();

			vkCmdPipelineBarrier2(CmdBuffer->CurrentCommandBuffer->CommandBuffer, &depInfo);
		}

		MemoryBarriers.clear();
		ImageBarriers.clear();
		BufferBarriers.clear();
	}


    VkImageSubresourceRange FVulkanPipelineBarrier::MakeSubresourceRange(VkImageAspectFlags AspectMask, uint32 FirstMip, uint32 NumMips, uint32 FirstLayer, uint32 NumLayers)
    {
		VkImageSubresourceRange SubresourceRange = {};
		SubresourceRange.aspectMask = AspectMask;
		SubresourceRange.baseMipLevel = FirstMip;
		SubresourceRange.levelCount = NumMips;
		SubresourceRange.baseArrayLayer = FirstLayer;
		SubresourceRange.layerCount = NumLayers;

		return SubresourceRange;
    }
}
