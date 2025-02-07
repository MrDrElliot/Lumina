#include "VulkanRenderAPI.h"
#include "imgui.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanMacros.h"
#include "VulkanPipeline.h"
#include "VulkanRenderContext.h"
#include "Core/Application/Application.h"
#include "Core/Windows/Window.h"
#include "Renderer/Material.h"
#include "Log/Log.h"

namespace Lumina
{
	
    FVulkanRenderAPI::FVulkanRenderAPI(const FRenderConfig& InConfig)
    {
    }

    FVulkanRenderAPI::~FVulkanRenderAPI()
    {
    	LOG_TRACE("Vulkan Render API: Shutting Down");
    	
    	delete RenderContext;
    	
    }
	
    void FVulkanRenderAPI::BeginFrame()
    {
        GetRenderContext()->GetSwapchain()->BeginFrame();
		GetRenderContext()->SetCommandBufferForFrame(GetRenderContext()->GetSwapchain()->GetCurrentFrameIndex());
		BeginCommandRecord();
    }

    void FVulkanRenderAPI::EndFrame()
    {
    	GetRenderContext()->GetSwapchain()->EndFrame();
    }

    void FVulkanRenderAPI::BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor)
    {
	    FRenderer::Submit([&, Attachments, ClearColor]
	    {
	    	TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
	    	
			VkRenderingAttachmentInfo				DepthAttachment = {};
			std::vector<VkRenderingAttachmentInfo>	ColorAttachments = {};
	    	uint32 RenderHeight =	GetRenderContext()->GetSwapchain()->GetSpecs().Extent.y;
			uint32 RenderWidth =	GetRenderContext()->GetSwapchain()->GetSpecs().Extent.x;
	    	
			for (TRefPtr<FImage> attachment : Attachments)
			{
				TRefPtr<FVulkanImage> VkTarget = RefPtrCast<FVulkanImage>(attachment);
				FImageSpecification target_spec = VkTarget->GetSpecification();

				if (target_spec.Usage == EImageUsage::RENDER_TARGET)
				{
					VkImageMemoryBarrier TargetBarrier = {};
					TargetBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					TargetBarrier.image = VkTarget->GetImage();
					TargetBarrier.oldLayout = (VkImageLayout)VkTarget->GetLayout();
					TargetBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					TargetBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
					TargetBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					TargetBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					TargetBarrier.subresourceRange.baseArrayLayer = 0;
					TargetBarrier.subresourceRange.baseMipLevel = 0;
					TargetBarrier.subresourceRange.layerCount = 1;
					TargetBarrier.subresourceRange.levelCount = 1;

					vkCmdPipelineBarrier(VkCommandBuffer->GetCommandBuffer(),
						VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
						VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1, 
						&TargetBarrier
					);

					VkTarget->SetCurrentLayout(EImageLayout::COLOR_ATTACHMENT);

					VkRenderingAttachmentInfo ColorAttachment = {};
					ColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
					ColorAttachment.imageView = VkTarget->GetImageView();
					ColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					ColorAttachment.loadOp = (ClearColor.a == 0.0f) ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
					ColorAttachment.clearValue = {{{ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a}}};
					ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

					ColorAttachments.push_back(ColorAttachment);
				}
				else if (target_spec.Usage == EImageUsage::DEPTH_BUFFER)
				{
					DepthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
					DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
					DepthAttachment.imageView = VkTarget->GetImageView();
					DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					DepthAttachment.clearValue.color = {{0,0,0,1}};
					DepthAttachment.loadOp = (ClearColor.a == 0.0f) ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
					DepthAttachment.clearValue.depthStencil.depth = 0.0f;
				}
			}
	    	
			VkRenderingInfo RenderingInfo = {};
			RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			RenderingInfo.renderArea = { { 0, 0 }, { RenderWidth, RenderHeight } };
			RenderingInfo.layerCount = 1;
			RenderingInfo.colorAttachmentCount = (uint32_t)ColorAttachments.size();
			RenderingInfo.pColorAttachments = ColorAttachments.data();
			RenderingInfo.pDepthAttachment = DepthAttachment.imageView ? &DepthAttachment : nullptr;
			RenderingInfo.pStencilAttachment = nullptr;



			VkRect2D scissor = { {0,0}, {RenderWidth, RenderHeight} };
			VkViewport viewport = { 0, (float)RenderHeight, (float)RenderWidth, -(float)RenderHeight, 0.0f, 1.0f};
			vkCmdSetScissor(VkCommandBuffer->GetCommandBuffer(), 0, 1, &scissor);
			vkCmdSetViewport(VkCommandBuffer->GetCommandBuffer(), 0, 1, &viewport);
			vkCmdBeginRendering(VkCommandBuffer->GetCommandBuffer(), &RenderingInfo);
		});
    }

    void FVulkanRenderAPI::EndRender()
    {
    	FRenderer::Submit([this]
    	{
    		TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
			vkCmdEndRendering(VkCommandBuffer->GetCommandBuffer());
		});
    }

    void FVulkanRenderAPI::WaitDevice()
    {
    	VK_CHECK(vkDeviceWaitIdle(GetRenderContext<FVulkanRenderContext>()->GetDevice()));
    }
	
    ERHIInterfaceType FVulkanRenderAPI::GetRHIInterfaceType()
    {
	    return ERHIInterfaceType::Vulkan;
    }

    void FVulkanRenderAPI::Initialize(const FRenderConfig& InConfig)
    {
    	LOG_TRACE("Vulkan Render API: Initializing");

    	Config = InConfig;
    	RenderContext = new FVulkanRenderContext(InConfig);
    	RenderContext->Initialize();
    }

    void FVulkanRenderAPI::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
    		FRenderer::Submit([&, BarrierInfo]
    		{

    			TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();

				std::vector<VkMemoryBarrier2> MemoryBarriers;
				std::vector<VkImageMemoryBarrier2> ImageBarriers;
				
				VkDependencyInfo dependency = {};
				dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				
				for (auto& buffer_barrier : BarrierInfo.BufferBarriers)
				{
				
					VkMemoryBarrier2 VkBarrier = {};
					VkBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
					VkBarrier.srcStageMask =  buffer_barrier.second.src_stages;
					VkBarrier.dstStageMask =  buffer_barrier.second.dst_stages;
					VkBarrier.srcAccessMask = buffer_barrier.second.src_access_mask;
					VkBarrier.dstAccessMask = buffer_barrier.second.dst_access_mask;
				
					MemoryBarriers.push_back(VkBarrier);
				}
				
				for (auto& image_barrier : BarrierInfo.ImageBarriers)
				{
					TRefPtr<FImage> image = image_barrier.first;
					TRefPtr<FVulkanImage> vk_image = RefPtrCast<FVulkanImage>(image);
				
					VkImageMemoryBarrier2 VkBarrier = {};
					VkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
					VkBarrier.srcStageMask = image_barrier.second.src_stages;
					VkBarrier.dstStageMask = image_barrier.second.dst_stages;
					VkBarrier.srcAccessMask = image_barrier.second.src_access_mask;
					VkBarrier.dstAccessMask = image_barrier.second.dst_access_mask;
					VkBarrier.image = vk_image->GetImage();
					VkBarrier.oldLayout = (VkImageLayout)vk_image->GetLayout();
					VkBarrier.newLayout = (VkImageLayout)image_barrier.second.new_image_layout;
					VkBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					VkBarrier.subresourceRange.baseArrayLayer = 0;
					VkBarrier.subresourceRange.layerCount = image->GetSpecification().ArrayLayers;
					VkBarrier.subresourceRange.baseMipLevel = 0;
					VkBarrier.subresourceRange.levelCount = image->GetSpecification().MipLevels;
				
					vk_image->SetCurrentLayout(image_barrier.second.new_image_layout);
				
					ImageBarriers.push_back(VkBarrier);
				}	
				
				dependency.memoryBarrierCount = (uint32)MemoryBarriers.size();
				dependency.pMemoryBarriers = MemoryBarriers.data();
				dependency.imageMemoryBarrierCount = (uint32)ImageBarriers.size();
				dependency.pImageMemoryBarriers = ImageBarriers.data();
				
				vkCmdPipelineBarrier2(
					VkCommandBuffer->GetCommandBuffer(),
					&dependency
			);
		});
    }

    void FVulkanRenderAPI::BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets)
    {
    	FRenderer::Submit([&, Set, Pipeline, SetIndex]
    	{

    		TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();

			TRefPtr<FVulkanPipeline> vk_pipeline = RefPtrCast<FVulkanPipeline>(Pipeline);
			TRefPtr<FVulkanDescriptorSet> vk_set = RefPtrCast<FVulkanDescriptorSet>(Set);
			VkDescriptorSet RawSet = vk_set->GetSet();
			
			VkPipelineBindPoint BindPoint = {};
			
			switch (Pipeline->GetSpecification().type)
			{
				case EPipelineType::GRAPHICS:			BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		break;
				case EPipelineType::COMPUTE:			BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;			break;
				case EPipelineType::RAY_TRACING:		BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;	break;
			}

    		/*// Check if there are any dynamic descriptors
  			uint32 dynamicOffsetCount = 0;//(uint32)DynamicOffsets.size();
  			const uint32* pDynamicOffsets = dynamicOffsetCount > 0 ? DynamicOffsets.data() : nullptr;*/
			
			vkCmdBindDescriptorSets(VkCommandBuffer->GetCommandBuffer(), BindPoint, vk_pipeline->GetPipelineLayout(), SetIndex, 1, &RawSet, 0, nullptr);
		});
    }

    void FVulkanRenderAPI::CopyToSwapchain(TRefPtr<FImage> ImageToCopy)
    {
    	FRenderer::Submit([&, ImageToCopy]
    	{
    		TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();

    		TRefPtr<FVulkanImage> vk_image = ImageToCopy.As<FVulkanImage>();
			TRefPtr<FImage> swapchain_image = GetRenderContext()->GetSwapchain<FVulkanSwapchain>()->GetCurrentImage();
			glm::uvec3 swapchain_resolution = swapchain_image->GetSpecification().Extent;
			glm::uvec3 src_image_resolution = ImageToCopy->GetSpecification().Extent;
						
			VkImageBlit ImageBlit = {};
			ImageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageBlit.srcSubresource.mipLevel = 0;
			ImageBlit.srcSubresource.baseArrayLayer = 0;
			ImageBlit.srcSubresource.layerCount = 1;
			ImageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageBlit.dstSubresource.mipLevel = 0;
			ImageBlit.dstSubresource.baseArrayLayer = 0;
			ImageBlit.dstSubresource.layerCount = 1;
			ImageBlit.srcOffsets[0] = { 0, 0, 0 };
			ImageBlit.srcOffsets[1] = { (int32)src_image_resolution.x, (int32)src_image_resolution.y, 1 };
			ImageBlit.dstOffsets[0] = { 0, 0, 0 };
			ImageBlit.dstOffsets[1] = { (int32)swapchain_resolution.x, (int32)swapchain_resolution.y, 1 };
    		
						
			vkCmdBlitImage(
				VkCommandBuffer->GetCommandBuffer(),
				vk_image->GetImage(),
				(VkImageLayout)vk_image->GetLayout(),
				RefPtrCast<FVulkanImage>(swapchain_image)->GetImage(),
				(VkImageLayout)VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&ImageBlit,
				VK_FILTER_LINEAR
			);
    		
    	});
    }

    void FVulkanRenderAPI::BindPipeline(TRefPtr<FPipeline> Pipeline)
    {
    	FRenderer::Submit([&, Pipeline]
    	{
    		TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
    		TRefPtr<FVulkanPipeline> VkPipeline = Pipeline.As<FVulkanPipeline>();
    		vkCmdBindPipeline(VkCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->GetPipeline());
    	});
    }

    void FVulkanRenderAPI::ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value)
    {
        FRenderer::Submit([&, Image, Value]
        {
        	TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
		
            Image->SetLayout
        	(
        		VkCommandBuffer,
        		EImageLayout::TRANSFER_DST,
        		EPipelineStage::TOP_OF_PIPE,
                EPipelineStage::TRANSFER
            );
            
            TRefPtr<FVulkanImage> VkImage = RefPtrCast<FVulkanImage>(Image);
            
            VkClearColorValue ClearColorValue = {};
            ClearColorValue.float32[0] = Value.r;
            ClearColorValue.float32[1] = Value.g;
            ClearColorValue.float32[2] = Value.b;
            ClearColorValue.float32[3] = Value.a;

            VkImageSubresourceRange Range = {};
            Range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            Range.baseMipLevel = 0;
            Range.levelCount = 1;
            Range.baseArrayLayer = 0;
            Range.layerCount = 1;

            vkCmdClearColorImage(VkCommandBuffer->GetCommandBuffer(),  VkImage->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue, 1, &Range);

            Image->SetLayout(VkCommandBuffer,
                EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE
            );
        });
    }

    void FVulkanRenderAPI::DrawIndexed(TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer)
    {
		FRenderer::Submit([&, VertexBuffer, IndexBuffer]
		{
			TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
			TRefPtr<FVulkanBuffer> VkVertexBuffer = RefPtrCast<FVulkanBuffer>(VertexBuffer);
			TRefPtr<FVulkanBuffer> VkIndexBuffer = RefPtrCast<FVulkanBuffer>(IndexBuffer);
			
			VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			VkDeviceSize Offsets[] = {0};
			
			vkCmdBindVertexBuffers(VkCommandBuffer->GetCommandBuffer(), 0, 1, &BindBuffer, Offsets);
			vkCmdBindIndexBuffer(VkCommandBuffer->GetCommandBuffer(), VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

			uint32 IndexCount = VkIndexBuffer->GetSpecification().Size / sizeof(uint32);

			vkCmdDrawIndexed(VkCommandBuffer->GetCommandBuffer(), IndexCount, 1, 0, 0, 0);
		});
    }

    void FVulkanRenderAPI::DrawVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
    	FRenderer::Submit([this, Vertices, Instances, FirstVertex, FirstInstance]
    	{
    		TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
    		vkCmdDraw(VkCommandBuffer->GetCommandBuffer(), Vertices, Instances, FirstVertex, FirstInstance);
    	});
    }
	
	void FVulkanRenderAPI::PushConstants(TRefPtr<FPipeline> Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data)
    {
    	AssertMsg(Data, "Attempted to push a constant with invalid data");

    	FRenderer::Submit([this, Pipeline, ShaderStage, Offset, Size, DataCopy = TVector<uint8>(static_cast<const uint8*>(Data), static_cast<const uint8*>(Data) + Size)]
		{
			static constexpr TArray<VkShaderStageFlags, 3> ShaderStageMap =
			{
				VK_SHADER_STAGE_VERTEX_BIT,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				VK_SHADER_STAGE_ALL
			};

    		TRefPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
    		TRefPtr<FVulkanPipeline> VkPipeline = Pipeline.As<FVulkanPipeline>();
			VkShaderStageFlags StageFlags = ShaderStageMap[static_cast<int>(ShaderStage)];
			vkCmdPushConstants(VkCommandBuffer->GetCommandBuffer(), VkPipeline->GetPipelineLayout(), StageFlags, Offset, Size, DataCopy.data());
		});
    }
	
    void FVulkanRenderAPI::BeginCommandRecord()
    {
        FRenderer::Submit([]
        {
            FRenderer::GetCommandBuffer()->Reset();
            FRenderer::GetCommandBuffer()->Begin();
        });
    }

    void FVulkanRenderAPI::EndCommandRecord()
    {
        FRenderer::Submit([]
        {
            FRenderer::GetCommandBuffer()->End(); 
        });
    }

    void FVulkanRenderAPI::ExecuteCurrentCommands()
    {
        FRenderer::Submit([&]
        {
            VkPipelineStageFlags StageMasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            TRefPtr<FVulkanCommandBuffer> VkCmdBuffer = GetRenderContext()->GetCommandBuffer().As<FVulkanCommandBuffer>();
        	uint32 Test = GetRenderContext()->GetSwapchain()->GetCurrentFrameIndex();
            VkSemaphore AquireSemaphore = GetRenderContext()->GetSwapchain<FVulkanSwapchain>()->GetAquireSemaphore();
            VkSemaphore PresentSemaphore = GetRenderContext()->GetSwapchain<FVulkanSwapchain>()->GetPresentSemaphore();

            VkSubmitInfo SubmitInfo = {};
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = &VkCmdBuffer->GetCommandBuffer();
            SubmitInfo.signalSemaphoreCount = 1;
            SubmitInfo.pSignalSemaphores = &PresentSemaphore;
            SubmitInfo.waitSemaphoreCount = 1;
            SubmitInfo.pWaitSemaphores = &AquireSemaphore;
            SubmitInfo.pWaitDstStageMask = StageMasks;

        	TRefPtr<FVulkanSwapchain> VkSwapchain = GetRenderContext()->GetSwapchain<FVulkanSwapchain>();
            VK_CHECK(vkQueueSubmit(GetRenderContext<FVulkanRenderContext>()->GetGeneralQueue(), 1, &SubmitInfo, VkSwapchain->GetCurrentFence()));
            
        });
    }
}
