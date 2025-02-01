#include "VulkanRenderAPI.h"
#include "imgui.h"
#include "VulkanBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanMacros.h"
#include "VulkanPipeline.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"
#include "Core/Application/Application.h"
#include "Core/Windows/Window.h"
#include "Renderer/Material.h"
#include "Log/Log.h"
#include "Renderer/PipelineLibrary.h"

namespace Lumina
{

    VkDescriptorPool FVulkanRenderAPI::DescriptorPool = VK_NULL_HANDLE;
    
    FVulkanRenderAPI::FVulkanRenderAPI(const FRenderConfig& InConfig)
    {
        LOG_TRACE("Vulkan Render API: Initializing");

        Config = InConfig;

        RenderContext = MakeSharedPtr<FVulkanRenderContext>(InConfig);
        Swapchain = RenderContext->GetSwapchain();

        CommandBuffers.resize(Swapchain->GetSpecs().FramesInFlight);
        CommandBuffers.shrink_to_fit();

        for (auto& Buffer : CommandBuffers)
        {
            Buffer = MakeRefPtr<FVulkanCommandBuffer>(ECommandBufferLevel::PRIMARY, ECommandBufferType::GENERAL, ECommandType::GENERAL);
        }

        if (DescriptorPool == VK_NULL_HANDLE)
        {
            glm::uint32 Count = 100 * InConfig.FramesInFlight;

            std::vector<VkDescriptorPoolSize> PoolSizes =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER,							Count }, 
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,			Count },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,					Count },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,					Count },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,					Count },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,			Count },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,					Count },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,			Count },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,				Count }
            };

            VkDescriptorPoolCreateInfo PoolInfo = {};
            PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            PoolInfo.maxSets = 1000;
            PoolInfo.poolSizeCount = (uint32)PoolSizes.size();
            PoolInfo.pPoolSizes = PoolSizes.data();
            PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            vkCreateDescriptorPool(RenderContext->GetDevice(), &PoolInfo, nullptr, &DescriptorPool);

        }
    }

    FVulkanRenderAPI::~FVulkanRenderAPI()
    {
    	LOG_TRACE("Vulkan Render API: Shutting Down");
    	
    	CommandBuffers.clear();
    	Swapchain->Release();
    	
    	vkDestroyDescriptorPool(FVulkanRenderContext::GetDevice(), DescriptorPool, nullptr);
    	
    	RenderContext->Destroy();
    }
	
    void FVulkanRenderAPI::BeginFrame()
    {
        Swapchain->BeginFrame();
		CurrentCommandBuffer = CommandBuffers[Swapchain->GetCurrentFrameIndex()];
		BeginCommandRecord();
    }

    void FVulkanRenderAPI::EndFrame()
    {
        Swapchain->EndFrame();
    }

    void FVulkanRenderAPI::BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor)
    {
	    FRenderer::Submit([&, Attachments, ClearColor]
	    {
			VkRenderingAttachmentInfo				DepthAttachment = {};
			std::vector<VkRenderingAttachmentInfo>	ColorAttachments = {};
	    	uint32 RenderHeight =	FRenderer::GetSwapchain()->GetSpecs().Extent.y;
			uint32 RenderWidth =	FRenderer::GetSwapchain()->GetSpecs().Extent.x;
	    	
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

					vkCmdPipelineBarrier(CurrentCommandBuffer->GetCommandBuffer(),
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
			vkCmdSetScissor(CurrentCommandBuffer->GetCommandBuffer(), 0, 1, &scissor);
			vkCmdSetViewport(CurrentCommandBuffer->GetCommandBuffer(), 0, 1, &viewport);
			vkCmdBeginRendering(CurrentCommandBuffer->GetCommandBuffer(), &RenderingInfo);
		});
    }

    void FVulkanRenderAPI::EndRender()
    {
    	FRenderer::Submit([this]
    	{
			vkCmdEndRendering(CurrentCommandBuffer->GetCommandBuffer());
		});
    }

    void FVulkanRenderAPI::WaitDevice()
    {
        VK_CHECK(vkDeviceWaitIdle(RenderContext->GetDevice()));
    }

    TRefPtr<FSwapchain> FVulkanRenderAPI::GetSwapchain()
    {
    	return Swapchain;
    }

    TRefPtr<FImage> FVulkanRenderAPI::GetSwapchainImage()
    {
        return Swapchain->GetCurrentImage();
    }

    ERHIInterfaceType FVulkanRenderAPI::GetRHIInterfaceType()
    {
	    return ERHIInterfaceType::Vulkan;
    }

    void FVulkanRenderAPI::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
    		FRenderer::Submit([&, BarrierInfo]
    		{
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
					CurrentCommandBuffer->GetCommandBuffer(),
					&dependency
			);
		});
    }

    void FVulkanRenderAPI::BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets)
    {
    	FRenderer::Submit([&, Set, Pipeline, SetIndex]
    	{
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
			
			vkCmdBindDescriptorSets(CurrentCommandBuffer->GetCommandBuffer(), BindPoint, vk_pipeline->GetPipelineLayout(), SetIndex, 1, &RawSet, 0, nullptr);
		});
    }

    void FVulkanRenderAPI::CopyToSwapchain(TRefPtr<FImage> ImageToCopy)
    {
    	FRenderer::Submit([&, ImageToCopy]
    	{
    		TRefPtr<FVulkanImage> vk_image = RefPtrCast<FVulkanImage>(ImageToCopy);
			TRefPtr<FImage> swapchain_image = Swapchain->GetCurrentImage();
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
				CurrentCommandBuffer->GetCommandBuffer(),
				vk_image->GetImage(),
				(VkImageLayout)vk_image->GetLayout(),
				RefPtrCast<FVulkanImage>(GetSwapchainImage())->GetImage(),
				(VkImageLayout)VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&ImageBlit,
				VK_FILTER_LINEAR
			);
    		
    	});
    }

    void FVulkanRenderAPI::BindPipeline(TRefPtr<FPipeline> Pipeline)
    {
    	TRefPtr<FVulkanPipeline> VkPipeline = RefPtrCast<FVulkanPipeline>(Pipeline);
    	FRenderer::Submit([&, VkPipeline]
    	{
    		vkCmdBindPipeline(CurrentCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->GetPipeline());
    		CurrentBoundPipeline = VkPipeline;
    	});
    }

    void FVulkanRenderAPI::ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value)
    {
        FRenderer::Submit([&, Image, Value]
        {
            Image->SetLayout(
                        CurrentCommandBuffer,
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

            vkCmdClearColorImage(CurrentCommandBuffer->GetCommandBuffer(),  VkImage->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue, 1, &Range);

            Image->SetLayout(CurrentCommandBuffer,
                EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE
            );
        });
    }

    void FVulkanRenderAPI::RenderMeshIndexed(TRefPtr<FPipeline> Pipeline, TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer, FMiscData Data)
    {
		FRenderer::Submit([&, Pipeline, VertexBuffer, IndexBuffer, Data]
		{
			VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
			TRefPtr<FVulkanBuffer> VkVertexBuffer = RefPtrCast<FVulkanBuffer>(VertexBuffer);
			TRefPtr<FVulkanBuffer> VkIndexBuffer = RefPtrCast<FVulkanBuffer>(IndexBuffer);
			TRefPtr<FVulkanPipeline> VkPipeline = RefPtrCast<FVulkanPipeline>(Pipeline);
			
			VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			VkDeviceSize Offsets[] = {0};


			
			vkCmdPushConstants(Buffer, VkPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data); 
			vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, Offsets);
			vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(Buffer, 36, 1, 0, 0, 0);

			delete Data.Data;
		});
    }

    void FVulkanRenderAPI::RenderVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
    	FRenderer::Submit([this, Vertices, Instances, FirstVertex, FirstInstance]
    	{
    		VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
    		vkCmdDraw(Buffer, Vertices, Instances, FirstVertex, FirstInstance);
    	});
    }

	void FVulkanRenderAPI::RenderStaticMeshWithMaterial(const TRefPtr<FPipeline>& Pipeline, const TSharedPtr<AStaticMesh>& StaticMesh, const TRefPtr<FMaterial>& Material)
    {
		FRenderer::Submit([this, Pipeline, StaticMesh]
		{
	    	VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
	    	TRefPtr<FVulkanBuffer> VkVertexBuffer = RefPtrCast<FVulkanBuffer>(StaticMesh->GetVertexBuffer());
	    	TRefPtr<FVulkanBuffer> VkIndexBuffer = RefPtrCast<FVulkanBuffer>(StaticMesh->GetIndexBuffer());
	    	TRefPtr<FVulkanPipeline> VkPipeline = RefPtrCast<FVulkanPipeline>(Pipeline);
						
	    	VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
	    	VkDeviceSize Offsets[] = {0};
			
	    	vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, Offsets);
	    	vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	    	vkCmdDrawIndexed(Buffer, (uint32)StaticMesh->GetMeshData().Indices.size(), 1, 0, 0, 0);

		});
      }

    void FVulkanRenderAPI::RenderStaticMesh(const TRefPtr<FPipeline>& Pipeline, TSharedPtr<AStaticMesh> StaticMesh, uint32 InstanceCount)
    {
    	if(StaticMesh == nullptr)
    	{
    		LOG_ERROR("Submitted an invalid StaticMesh for Render!");
    		return;
    	}
    	
    	FRenderer::BindPipeline(Pipeline);
    	
    	FRenderer::Submit([this, StaticMesh, InstanceCount]
 		{
			 VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
			 TRefPtr<FVulkanBuffer> VkVertexBuffer = RefPtrCast<FVulkanBuffer>(StaticMesh->GetVertexBuffer());
			 TRefPtr<FVulkanBuffer> VkIndexBuffer = RefPtrCast<FVulkanBuffer>(StaticMesh->GetIndexBuffer());
									
			 VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			 VkDeviceSize Offsets[] = {0};
						
			 vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, Offsets);
			 vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
			 vkCmdDrawIndexed(Buffer, (uint32)StaticMesh->GetMeshData().Indices.size(), InstanceCount, 0, 0, 0);
 				
 		});
    }

    std::vector<VkDescriptorSet> FVulkanRenderAPI::AllocateDescriptorSets(VkDescriptorSetLayout InLayout, uint32 InCount)
    {
		auto Device = FVulkanRenderContext::GetDevice();
    	
    	std::vector<VkDescriptorSet> sets(InCount);

    	VkDescriptorSetAllocateInfo AllocateInfo = {};
    	AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    	AllocateInfo.descriptorPool = DescriptorPool;
    	AllocateInfo.descriptorSetCount = InCount;
    	AllocateInfo.pSetLayouts = &InLayout;

    	VK_CHECK(vkAllocateDescriptorSets(Device, &AllocateInfo, sets.data()));

    	return sets;
    }

    void FVulkanRenderAPI::FreeDescriptorSets(std::vector<VkDescriptorSet> InSets)
    {
    	auto Device = FVulkanRenderContext::GetDevice();

    	vkFreeDescriptorSets(Device, DescriptorPool, (uint32)InSets.size(), InSets.data());
    }

	void FVulkanRenderAPI::PushConstants(EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data)
    {
    	AssertMsg(Data, "Attempted to push a constant with invalid data");
    
    	// Copy the data into the lambda to ensure it is valid when executed
    	std::vector<uint8_t> dataCopy((uint8*)Data, (uint8*)Data + Size);

    	FRenderer::Submit([this, ShaderStage, Offset, Size, dataCopy = std::move(dataCopy)]
		{
			VkShaderStageFlags StageFlags = 0;
			if (ShaderStage == EShaderStage::VERTEX)
			{
				StageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			}
    		else if (ShaderStage == EShaderStage::FRAGMENT)
			{
				StageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}
    		else if (ShaderStage == EShaderStage::ALL)
			{
				StageFlags |= VK_SHADER_STAGE_ALL;
			}

			vkCmdPushConstants(CurrentCommandBuffer->GetCommandBuffer(), CurrentBoundPipeline->GetPipelineLayout(), StageFlags, Offset, Size, dataCopy.data());
		});
    }

    void FVulkanRenderAPI::RenderMeshTasks(TRefPtr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data)
    {
    	FRenderer::Submit([&, Pipeline, Data]()
    	{
			TRefPtr<FVulkanPipeline> vk_pipeline = RefPtrCast<FVulkanPipeline>(Pipeline);
			
			if (Data.Size)
			{
				vkCmdPushConstants(CurrentCommandBuffer->GetCommandBuffer(), vk_pipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data);
				delete[] Data.Data;
			}
			vkCmdBindPipeline(CurrentCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->GetPipeline());
			//vkCmdDrawMeshTasksEXT(CurrentCommandBuffer->GetCommandBuffer(), Dimensions.x, Dimensions.y, Dimensions.z);
		});
    }

    TRefPtr<FCommandBuffer> FVulkanRenderAPI::GetCommandBuffer()
    {
        return CurrentCommandBuffer;
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

            auto VkCmdBuffer = RefPtrCast<FVulkanCommandBuffer>(GetCommandBuffer());

            auto AquireSemaphore = Swapchain->GetAquireSemaphore();
            auto PresentSemaphore = Swapchain->GetPresentSemaphore();

            VkSubmitInfo SubmitInfo = {};
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = &VkCmdBuffer->GetCommandBuffer();
            SubmitInfo.signalSemaphoreCount = 1;
            SubmitInfo.pSignalSemaphores = &PresentSemaphore;
            SubmitInfo.waitSemaphoreCount = 1;
            SubmitInfo.pWaitSemaphores = &AquireSemaphore;
            SubmitInfo.pWaitDstStageMask = StageMasks;

            FVulkanRenderContext& Context = FVulkanRenderContext::Get();
            
            vkQueueSubmit(Context.GetGeneralQueue(), 1, &SubmitInfo, Swapchain->GetCurrentFence());
            
        });
    }
}
