#include "VulkanRenderAPI.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanImage.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Renderer/RHIIncl.h"
#include "Core/Windows/Window.h"
#include "Log/Log.h"
#include "Renderer/RenderPass.h"

namespace Lumina
{
	
    FVulkanRenderAPI::FVulkanRenderAPI()
    {
    }

    FVulkanRenderAPI::~FVulkanRenderAPI()
    {
    	LOG_TRACE("Vulkan Render API: Shutting Down");
    	
    	FMemory::Delete(RenderContext);
    	
    }
	
    void FVulkanRenderAPI::BeginFrame()
    {
        GetRenderContext()->GetSwapchain()->BeginFrame();
		GetRenderContext()->SetCommandBufferForFrame(GetRenderContext()->GetSwapchain()->GetFrameIndex());
		BeginCommandRecord();
    }

    void FVulkanRenderAPI::Present()
    {
    	GetRenderContext()->GetSwapchain()->Present();
    }

    void FVulkanRenderAPI::BeginRender(const FRenderPassBeginInfo& Info)
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
	    	
    	VkRenderingAttachmentInfo				DepthAttachment = {};
    	TVector<VkRenderingAttachmentInfo>		ColorAttachments = {};
	    	
    	for (FRHIImage attachment : Info.Attachments)
    	{
			TRefCountPtr<FVulkanImage> VkTarget = attachment.As<FVulkanImage>();
			EImageUsage ImageUsage = VkTarget->GetSpecification().Usage;

			switch (ImageUsage)
			{
				case EImageUsage::TEXTURE: break;
				case EImageUsage::RENDER_TARGET:
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

						VkClearColorValue ClearColorValue = {};
						ClearColorValue.float32[0] = Info.ClearColor.R;
						ClearColorValue.float32[1] = Info.ClearColor.G;
						ClearColorValue.float32[2] = Info.ClearColor.B;
						ClearColorValue.float32[3] = Info.ClearColor.A;

						VkClearValue ClearValue = {};
						ClearValue.color = ClearColorValue;
						
						VkRenderingAttachmentInfo ColorAttachment = {};
						ColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
						ColorAttachment.imageView = VkTarget->GetImageView();
						ColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						ColorAttachment.loadOp = (Info.bClearValue) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
						ColorAttachment.clearValue = ClearValue;
						ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

						ColorAttachments.push_back(ColorAttachment);
					}
					break;
				case EImageUsage::DEPTH_BUFFER:
					{
						DepthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
						DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
						DepthAttachment.imageView = VkTarget->GetImageView();
						DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
						DepthAttachment.clearValue.color = {{0,0,0,1}};
						DepthAttachment.loadOp = (Info.bClearValue) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
						DepthAttachment.clearValue.depthStencil.depth = 0.0f;
					}
					break;
				case EImageUsage::RESOLVE: break;
				}
			}

	    VkRect2D RenderArea;
	    RenderArea.offset.x = Info.RenderAreaOffset.X;
	    RenderArea.offset.y = Info.RenderAreaOffset.Y;
	    RenderArea.extent.width = Info.RenderAreaExtent.X;
		RenderArea.extent.height = Info.RenderAreaExtent.Y;
	    
		VkRenderingInfo RenderingInfo = {};
		RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		RenderingInfo.renderArea = RenderArea;
		RenderingInfo.layerCount = 1;
		RenderingInfo.pColorAttachments = ColorAttachments.data();
		RenderingInfo.colorAttachmentCount = (uint32)ColorAttachments.size();
		RenderingInfo.pDepthAttachment = DepthAttachment.imageView ? &DepthAttachment : nullptr;
		RenderingInfo.pStencilAttachment = nullptr;

	    
		VkViewport Viewport;
	    Viewport.x = 0;
	    Viewport.y = Info.RenderAreaExtent.Y;
		Viewport.width = Info.RenderAreaExtent.X;
		Viewport.height = -Info.RenderAreaExtent.Y;
	    Viewport.maxDepth = 0.0f;
	    Viewport.minDepth = 1.0f;
	    
		vkCmdSetScissor(VkCommandBuffer->GetCommandBuffer(), 0, 1, &RenderArea);
		vkCmdSetViewport(VkCommandBuffer->GetCommandBuffer(), 0, 1, &Viewport);
		vkCmdBeginRendering(VkCommandBuffer->GetCommandBuffer(), &RenderingInfo);
    	
    }

    void FVulkanRenderAPI::EndRender()
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
		vkCmdEndRendering(VkCommandBuffer->GetCommandBuffer());
    }

    void FVulkanRenderAPI::WaitDevice()
    {
    	VK_CHECK(vkDeviceWaitIdle(GetRenderContext<FVulkanRenderContext>()->GetDevice()));
    }
	
    ERHIInterfaceType FVulkanRenderAPI::GetRHIInterfaceType()
    {
	    return ERHIInterfaceType::Vulkan;
    }

    void FVulkanRenderAPI::Initialize()
    {
    	LOG_TRACE("Vulkan Render API: Initializing");

    	RenderContext = FMemory::New<FVulkanRenderContext>();
    	RenderContext->Initialize();
    }

    void FVulkanRenderAPI::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();

		TVector<VkMemoryBarrier2> MemoryBarriers;
		TVector<VkImageMemoryBarrier2> ImageBarriers;
				
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
			FRHIImage image = image_barrier.first;
			TRefCountPtr<FVulkanImage> vk_image = image.As<FVulkanImage>();
		
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
				
    	vkCmdPipelineBarrier2(VkCommandBuffer->GetCommandBuffer(), &dependency);
    }

    void FVulkanRenderAPI::BindSet(const TRefCountPtr<FDescriptorSet>& Set, const FRHIPipeline& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets)
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();

    	VkPipelineLayout VulkanPipelineLayout = (VkPipelineLayout)Pipeline->GetPlatformPipelineLayout();
    	VkDescriptorSet VulanDescriptorSet = (VkDescriptorSet)Set->GetPlatformDescriptorSet();
			
    	VkPipelineBindPoint BindPoint = {};
			
		switch (Pipeline->GetSpecification().GetPipelineType())
		{
			case EPipelineType::GRAPHICS:			BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		break;
			case EPipelineType::COMPUTE:			BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;			break;
			case EPipelineType::RAY_TRACING:		BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;	break;
		}
    		
    	vkCmdBindDescriptorSets(VkCommandBuffer->GetCommandBuffer(), BindPoint, VulkanPipelineLayout, SetIndex, 1, &VulanDescriptorSet, 0, nullptr);
    }

    void FVulkanRenderAPI::CopyToSwapchain(FRHIImage ImageToCopy)
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();

    	TRefCountPtr<FVulkanImage> vk_image = ImageToCopy.As<FVulkanImage>();
		FRHIImage swapchain_image = GetRenderContext()->GetSwapchain<FVulkanSwapchain>()->GetCurrentImage();
		FVector3D swapchain_resolution = swapchain_image->GetSpecification().Extent;
		FVector3D src_image_resolution = ImageToCopy->GetSpecification().Extent;
						
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
		ImageBlit.srcOffsets[1] = { (int32)src_image_resolution.X, (int32)src_image_resolution.Y, 1 };
		ImageBlit.dstOffsets[0] = { 0, 0, 0 };
		ImageBlit.dstOffsets[1] = { (int32)swapchain_resolution.X, (int32)swapchain_resolution.Y, 1 };
    		
						
		vkCmdBlitImage(
			VkCommandBuffer->GetCommandBuffer(),
			vk_image->GetImage(),
			(VkImageLayout)vk_image->GetLayout(),
			swapchain_image.As<FVulkanImage>()->GetImage(),
			(VkImageLayout)VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&ImageBlit,
			VK_FILTER_LINEAR
		);
    }

    void FVulkanRenderAPI::BindPipeline(FRHIPipeline Pipeline)
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
    	VkPipeline VulkanPipeline = (VkPipeline)Pipeline->GetPlatformPipeline();
    		
    	vkCmdBindPipeline(VkCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanPipeline);
    }

    void FVulkanRenderAPI::ClearColor(const FRHIImage& Image, const glm::fvec4& Value)
    {
    	FRHICommandBuffer CommandBuffer = GetRenderContext()->GetCommandBuffer();
		
        Image->SetLayout
		(
     		CommandBuffer,
     		EImageLayout::TRANSFER_DST,
     		EPipelineStage::TOP_OF_PIPE,
            EPipelineStage::TRANSFER
        );

    	TRefCountPtr<FVulkanImage> VkImage = Image.As<FVulkanImage>();
            
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

    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = CommandBuffer.As<FVulkanCommandBuffer>();

    	vkCmdClearColorImage(VkCommandBuffer->GetCommandBuffer(),  VkImage->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue, 1, &Range);

        Image->SetLayout(CommandBuffer, EImageLayout::PRESENT_SRC, EPipelineStage::TRANSFER, EPipelineStage::BOTTOM_OF_PIPE);
    }

    void FVulkanRenderAPI::DrawIndexed(uint32 IndexCount, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
		TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
		vkCmdDrawIndexed(VkCommandBuffer->GetCommandBuffer(), IndexCount, Instances, FirstVertex, 0, FirstInstance);
    }

    void FVulkanRenderAPI::DrawVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
    	vkCmdDraw(VkCommandBuffer->GetCommandBuffer(), Vertices, Instances, FirstVertex, FirstInstance);
    }
	
	void FVulkanRenderAPI::PushConstants(FRHIPipeline Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data)
    {
    	AssertMsg(Data, "Attempted to push a constant with invalid data");
    	
		static constexpr TArray<VkShaderStageFlags, 3> ShaderStageMap =
		{
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			VK_SHADER_STAGE_ALL
		};

    	VkPipelineLayout VulkanPipelineLayout = (VkPipelineLayout)Pipeline->GetPlatformPipelineLayout();
    	
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
		VkShaderStageFlags StageFlags = ShaderStageMap[static_cast<int>(ShaderStage)];
    	
		vkCmdPushConstants(VkCommandBuffer->GetCommandBuffer(), VulkanPipelineLayout, StageFlags, Offset, Size, Data);
    }

	void FVulkanRenderAPI::SetShaderParameter(const FName& ParameterName, void* Data, uint32 Size)
	{
    	Assert(Data);
    	Assert(Size);
    	
    	FPipelineState* PipelineState = GetRenderContext()->GetPipelineState();
    	Assert(PipelineState);
    	  	
    	FPipelineState::FPipelineStateBuffer Buffer = PipelineState->GetBufferForDescriptor(ParameterName);
    	Assert(Buffer.Buffer);
    	
    	FRHIDescriptorSet DescriptorSet = PipelineState->GetDescriptorSetForDescriptor(ParameterName);
    	Assert(DescriptorSet);
    	
    	Buffer.Buffer->UploadData(0, Data, Size);
		  	
    	PipelineState->AddPendingDescriptorWrite(DescriptorSet, Buffer.Buffer, Buffer.DescriptorSetIndex);
	}

	void FVulkanRenderAPI::BindVertexBuffer(FRHIBuffer VertexBuffer)
	{
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
    	TRefCountPtr<FVulkanBuffer> VkVertexBuffer = VertexBuffer.As<FVulkanBuffer>();
    	VkDeviceSize Offsets[] = {0};
						
    	vkCmdBindVertexBuffers(VkCommandBuffer->GetCommandBuffer(), 0, 1, &VkVertexBuffer->GetBuffer(), Offsets);
	}

	void FVulkanRenderAPI::BindIndexBuffer(FRHIBuffer IndexBuffer)
	{
    	TRefCountPtr<FVulkanCommandBuffer> VkCommandBuffer = GetRenderContext()->GetCommandBuffer<FVulkanCommandBuffer>();
			
    	TRefCountPtr<FVulkanBuffer> VkIndexBuffer = IndexBuffer.As<FVulkanBuffer>();
						
    	vkCmdBindIndexBuffer(VkCommandBuffer->GetCommandBuffer(), VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void FVulkanRenderAPI::BeginCommandRecord()
    {
    	FRenderer::GetCommandBuffer()->Reset();
    	FRenderer::GetCommandBuffer()->Begin();
    }

    void FVulkanRenderAPI::EndCommandRecord()
    {
    	FRenderer::GetCommandBuffer()->End(); 
    }

    void FVulkanRenderAPI::ExecuteCurrentCommands()
    {
         VkPipelineStageFlags StageMasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

         TRefCountPtr<FVulkanCommandBuffer> VkCmdBuffer = GetRenderContext()->GetCommandBuffer().As<FVulkanCommandBuffer>();
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

    	 TRefCountPtr<FVulkanSwapchain> VkSwapchain = GetRenderContext()->GetSwapchain<FVulkanSwapchain>();
         VK_CHECK(vkQueueSubmit(GetRenderContext<FVulkanRenderContext>()->GetGeneralQueue(), 1, &SubmitInfo, VkSwapchain->GetCurrentFence()));
            
    }
}
