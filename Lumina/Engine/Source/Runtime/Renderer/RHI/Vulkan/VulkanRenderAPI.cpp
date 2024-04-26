#include "VulkanRenderAPI.h"
#include "imgui.h"
#include "VulkanBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanPipeline.h"
#include "backends/imgui_impl_vulkan.h"
#include "Source/Runtime/Assets/StaticMesh/StaticMesh.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/PipelineLibrary.h"

namespace Lumina
{

    VkDescriptorPool FVulkanRenderAPI::DescriptorPool = VK_NULL_HANDLE;
    
    FVulkanRenderAPI::FVulkanRenderAPI(const FRenderConfig& InConfig)
    {
        LE_LOG_INFO("Vulkan Render API: Initializing");

        Config = InConfig;

        RenderContext = std::make_shared<FVulkanRenderContext>(InConfig);
        Swapchain = RenderContext->GetSwapchain();

        CommandBuffers.resize(Swapchain->GetSpecs().FramesInFlight);
        CommandBuffers.shrink_to_fit();

        for (auto& Buffer : CommandBuffers)
        {
            Buffer = std::make_shared<FVulkanCommandBuffer>(ECommandBufferLevel::PRIMARY, ECommandBufferType::GENERAL, ECommandType::GENERAL);
        }

        if (DescriptorPool == VK_NULL_HANDLE)
        {
            glm::uint32 Count = UINT16_MAX * InConfig.FramesInFlight;

            std::vector<VkDescriptorPoolSize> PoolSizes =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER,					    Count }, 
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	    Count },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				Count },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				Count },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			    Count },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			    Count },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			Count }
            };

            VkDescriptorPoolCreateInfo PoolInfo = {};
            PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            PoolInfo.maxSets = 1000;
            PoolInfo.poolSizeCount = PoolSizes.size();
            PoolInfo.pPoolSizes = PoolSizes.data();
            PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            vkCreateDescriptorPool(RenderContext->GetDevice(), &PoolInfo, nullptr, &DescriptorPool);

        }
    }

    FVulkanRenderAPI::~FVulkanRenderAPI()
    {
        LE_LOG_WARN("Vulkan Render API: Shutting Down");

        vkDeviceWaitIdle(FVulkanRenderContext::GetDevice());

        for(auto& Buffer : CommandBuffers)
        {
            Buffer->Destroy();
        }

        vkDestroyDescriptorPool(FVulkanRenderContext::GetDevice(), DescriptorPool, nullptr);

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

    void FVulkanRenderAPI::BeginRender(std::vector<std::shared_ptr<FImage>> Attachments, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor)
    {
	    FRenderer::Submit([&, Attachments, RenderArea, RenderOffset, ClearColor]
	    {
	    	

			VkRenderingAttachmentInfo				DepthAttachment = {};
			std::vector<VkRenderingAttachmentInfo>	ColorAttachments = {};

			for (auto attachment : Attachments)
			{
				std::shared_ptr<FVulkanImage> VkTarget = std::dynamic_pointer_cast<FVulkanImage>(attachment);
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
					if (ClearColor.a != 0.0f)
					{
						ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					}
					else
					{
						ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
					}
					ColorAttachment.clearValue = {{ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a}};
					ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

					ColorAttachments.push_back(ColorAttachment);
				}
				else
				{
					DepthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
					DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
					DepthAttachment.imageView = VkTarget->GetImageView();
					DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					DepthAttachment.clearValue.color = {{0,0,0,1}};
					if (ClearColor.a != 0.0f)
					{
						DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					}
					else
					{
						DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
					}
					DepthAttachment.clearValue.depthStencil = { 0.0f, 0 };
				}
			}
			

			VkRenderingInfo RenderingInfo = {};
			RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			RenderingInfo.renderArea = { { RenderOffset.x, RenderOffset.y }, { RenderArea.x, RenderArea.y } };
			RenderingInfo.layerCount = 1;
			RenderingInfo.colorAttachmentCount = ColorAttachments.size();
			RenderingInfo.pColorAttachments = ColorAttachments.data();
			RenderingInfo.pDepthAttachment = DepthAttachment.imageView ? &DepthAttachment : nullptr;
			RenderingInfo.pStencilAttachment = nullptr;



			VkRect2D scissor = { {0,0}, {RenderArea.x, RenderArea.y} };
			VkViewport viewport = { 0, (glm::float32)RenderArea.y, (glm::float32)RenderArea.x, -(glm::float32)RenderArea.y, 0.0f, 1.0f};
			vkCmdSetScissor(CurrentCommandBuffer->GetCommandBuffer(), 0, 1, &scissor);
			vkCmdSetViewport(CurrentCommandBuffer->GetCommandBuffer(), 0, 1, &viewport);
			vkCmdBeginRendering(CurrentCommandBuffer->GetCommandBuffer(), &RenderingInfo);
		});
    }

    void FVulkanRenderAPI::EndRender()
    {
    	FRenderer::Submit([&]
    	{
			vkCmdEndRendering(CurrentCommandBuffer->GetCommandBuffer());
		});
    }

    void FVulkanRenderAPI::WaitDevice()
    {
        vkDeviceWaitIdle(RenderContext->GetDevice());
    }

    std::shared_ptr<FSwapchain> FVulkanRenderAPI::GetSwapchain()
    {
    	return Swapchain;
    }

    std::shared_ptr<FImage> FVulkanRenderAPI::GetSwapchainImage()
    {
        return Swapchain->GetCurrentImage();
    }

    void FVulkanRenderAPI::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
    		FRenderer::Submit([&, BarrierInfo]
    		{
				std::vector<VkMemoryBarrier2> memory_barriers;
				std::vector<VkImageMemoryBarrier2> image_barriers;
				
				VkDependencyInfo dependency = {};
				dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				
				for (auto& buffer_barrier : BarrierInfo.BufferBarriers)
				{
				
					VkMemoryBarrier2 vk_barrier = {};
					vk_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
					vk_barrier.srcStageMask =  buffer_barrier.second.src_stages;
					vk_barrier.dstStageMask =  buffer_barrier.second.dst_stages;
					vk_barrier.srcAccessMask = buffer_barrier.second.src_access_mask;
					vk_barrier.dstAccessMask = buffer_barrier.second.dst_access_mask;
				
					memory_barriers.push_back(vk_barrier);
				}
				
				for (auto& image_barrier : BarrierInfo.ImageBarriers)
				{
					std::shared_ptr<FImage> image = image_barrier.first;
					std::shared_ptr<FVulkanImage> vk_image = std::dynamic_pointer_cast<FVulkanImage>(image);
				
					VkImageMemoryBarrier2 vk_barrier = {};
					vk_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
					vk_barrier.srcStageMask = image_barrier.second.src_stages;
					vk_barrier.dstStageMask = image_barrier.second.dst_stages;
					vk_barrier.srcAccessMask = image_barrier.second.src_access_mask;
					vk_barrier.dstAccessMask = image_barrier.second.dst_access_mask;
					vk_barrier.image = vk_image->GetImage();
					vk_barrier.oldLayout = (VkImageLayout)vk_image->GetLayout();
					vk_barrier.newLayout = (VkImageLayout)image_barrier.second.new_image_layout;
					vk_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_barrier.subresourceRange.baseArrayLayer = 0;
					vk_barrier.subresourceRange.layerCount = image->GetSpecification().ArrayLayers;
					vk_barrier.subresourceRange.baseMipLevel = 0;
					vk_barrier.subresourceRange.levelCount = image->GetSpecification().MipLevels;
				
					vk_image->SetCurrentLayout(image_barrier.second.new_image_layout);
				
					image_barriers.push_back(vk_barrier);
				}	
				
				dependency.memoryBarrierCount = memory_barriers.size();
				dependency.pMemoryBarriers = memory_barriers.data();
				dependency.imageMemoryBarrierCount = image_barriers.size();
				dependency.pImageMemoryBarriers = image_barriers.data();
				
				vkCmdPipelineBarrier2(
					CurrentCommandBuffer->GetCommandBuffer(),
					&dependency
			);
		});
    }

    void FVulkanRenderAPI::BindSet(std::shared_ptr<FDescriptorSet> Set, std::shared_ptr<FPipeline> Pipeline, glm::uint8 Index)
    {
    	FRenderer::Submit([&, Set, Pipeline, Index]
    	{
			std::shared_ptr<FVulkanPipeline> vk_pipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
			std::shared_ptr<FVulkanDescriptorSet> vk_set = std::dynamic_pointer_cast<FVulkanDescriptorSet>(Set);
			VkDescriptorSet RawSet = vk_set->GetSet();
			
			VkPipelineBindPoint BindPoint = {};
			
			switch (Pipeline->GetSpecification().type)
			{
				case EPipelineType::GRAPHICS:			BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		break;
				case EPipelineType::COMPUTE:			BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;			break;
				case EPipelineType::RAY_TRACING:		BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;	break;
			}
			
			vkCmdBindDescriptorSets(CurrentCommandBuffer->GetCommandBuffer(), BindPoint, vk_pipeline->GetPipelineLayout(), Index, 1, &RawSet, 0, nullptr);
		});
    }

    void FVulkanRenderAPI::CopyToSwapchain(std::shared_ptr<FImage> ImageToCopy)
    {
    	FRenderer::Submit([&, ImageToCopy]
    	{
    		std::shared_ptr<FVulkanImage> vk_image = std::dynamic_pointer_cast<FVulkanImage>(ImageToCopy);
			std::shared_ptr<FImage> swapchain_image = Swapchain->GetCurrentImage();
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
			ImageBlit.srcOffsets[1] = { (glm::int32)src_image_resolution.x, (glm::int32)src_image_resolution.y, 1 };
			ImageBlit.dstOffsets[0] = { 0, 0, 0 };
			ImageBlit.dstOffsets[1] = { (glm::int32)swapchain_resolution.x, (glm::int32)swapchain_resolution.y, 1 };
    		
						
			vkCmdBlitImage(
				CurrentCommandBuffer->GetCommandBuffer(),
				vk_image->GetImage(),
				(VkImageLayout)vk_image->GetLayout(),
				std::dynamic_pointer_cast<FVulkanImage>(GetSwapchainImage())->GetImage(),
				(VkImageLayout)VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&ImageBlit,
				VK_FILTER_LINEAR
			);
    		
    	});
    }

    void FVulkanRenderAPI::BindPipeline(std::shared_ptr<FPipeline> Pipeline)
    {
    	FRenderer::Submit([&, Pipeline]
    	{
    		std::shared_ptr<FVulkanPipeline> VkPipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
    		vkCmdBindPipeline(CurrentCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->GetPipeline());
    	});
    }

    void FVulkanRenderAPI::ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value)
    {
        FRenderer::Submit([&, Image, Value]
        {

            Image->SetLayout(
                        CurrentCommandBuffer,
                        EImageLayout::TRANSFER_DST,
                        EPipelineStage::TOP_OF_PIPE,
                        EPipelineStage::TRANSFER
                    );
            
            std::shared_ptr<FVulkanImage> VkImage = std::dynamic_pointer_cast<FVulkanImage>(Image);
            
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

    void FVulkanRenderAPI::RenderMeshIndexed(std::shared_ptr<FPipeline> Pipeline, std::shared_ptr<FBuffer> VertexBuffer, std::shared_ptr<FBuffer> IndexBuffer, FMiscData Data)
    {
		FRenderer::Submit([&, Pipeline, VertexBuffer, IndexBuffer, Data]
		{
			VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
			std::shared_ptr<FVulkanBuffer> VkVertexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(VertexBuffer);
			std::shared_ptr<FVulkanBuffer> VkIndexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(IndexBuffer);
			std::shared_ptr<FVulkanPipeline> VkPipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
			
			VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			VkDeviceSize Offsets[] = {0};


			vkCmdPushConstants(Buffer, VkPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data); 
			vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, Offsets);
			vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(Buffer, 36, 1, 0, 0, 0);

			delete Data.Data;
		});
    }

    void FVulkanRenderAPI::RenderStaticMesh(std::shared_ptr<FPipeline> Pipeline, std::shared_ptr<LStaticMesh> StaticMesh, FMiscData Data)
    {
    	FRenderer::Submit([&, Pipeline, StaticMesh, Data]
		{
			VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
			std::shared_ptr<FVulkanBuffer> VkVertexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(StaticMesh->GetBuffers().first);
			std::shared_ptr<FVulkanBuffer> VkIndexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(StaticMesh->GetBuffers().second);
			std::shared_ptr<FVulkanPipeline> VkPipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
					
			VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			VkDeviceSize Offsets[] = {0};
		
			vkCmdPushConstants(Buffer, VkPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data); 
			vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, Offsets);
			vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		
			vkCmdDrawIndexed(Buffer, StaticMesh->GetMeshData().Indices.size(), 1, 0, 0, 0);

    		delete Data.Data;
		});
    }

    void FVulkanRenderAPI::RenderStaticMesh(std::shared_ptr<FPipeline> Pipeline, std::shared_ptr<LStaticMesh> StaticMesh)
    {
    	FRenderer::Submit([&, Pipeline, StaticMesh]
 		{
			 VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
			 std::shared_ptr<FVulkanBuffer> VkVertexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(StaticMesh->GetBuffers().first);
			 std::shared_ptr<FVulkanBuffer> VkIndexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(StaticMesh->GetBuffers().second);
			 std::shared_ptr<FVulkanPipeline> VkPipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
							
			 VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			 VkDeviceSize Offsets[] = {0};
				
			 vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, Offsets);
			 vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
				
			 vkCmdDrawIndexed(Buffer, StaticMesh->GetMeshData().Indices.size(), 1, 0, 0, 0);
 		
 		});
    }


    void FVulkanRenderAPI::RenderQuad(std::shared_ptr<FPipeline> Pipeline, FMiscData Data)
    {
    	FRenderer::Submit([&, Pipeline, Data]
    	{
			std::shared_ptr<FVulkanPipeline> VkPipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
			
			vkCmdPushConstants(CurrentCommandBuffer->GetCommandBuffer(), VkPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data);
			vkCmdBindPipeline(CurrentCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->GetPipeline());
			
			vkCmdDraw(CurrentCommandBuffer->GetCommandBuffer(), 6, 1, 0, 0);

    		if(Data.Data)
    		{
				delete[] Data.Data;
    		}
		});
    }

    void FVulkanRenderAPI::RenderQuad(std::shared_ptr<FPipeline> Pipeline, glm::uint32 Amount, FMiscData Data)
    {
    	FRenderer::Submit([&, Pipeline, Amount, Data]
    	{
    		std::shared_ptr<FVulkanPipeline> VkPipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
			
			if (Data.Size)
			{
				vkCmdPushConstants(CurrentCommandBuffer->GetCommandBuffer(), VkPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data);
				delete[] Data.Data;
			}
			vkCmdBindPipeline(CurrentCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->GetPipeline());
			vkCmdDraw(CurrentCommandBuffer->GetCommandBuffer(), 6, Amount, 0, 0);
		});
    }

    std::vector<VkDescriptorSet> FVulkanRenderAPI::AllocateDescriptorSets(VkDescriptorSetLayout InLayout, glm::uint32 InCount)
    {
		auto Device = FVulkanRenderContext::GetDevice();
    	
    	std::vector<VkDescriptorSet> sets(InCount);
    	sets.resize(InCount);

    	VkDescriptorSetAllocateInfo AllocateInfo = {};
    	AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    	AllocateInfo.descriptorPool = DescriptorPool;
    	AllocateInfo.descriptorSetCount = InCount;
    	AllocateInfo.pSetLayouts = &InLayout;

    	if (vkAllocateDescriptorSets(Device, &AllocateInfo, sets.data()) != VK_SUCCESS) 
    	{
    		LE_LOG_ERROR("Failed to allocate descriptor set. Possible issue: too many allocated descriptor sets.");
    		return std::vector<VkDescriptorSet>(0);
    	}

    	return sets;
    }

    void FVulkanRenderAPI::FreeDescriptorSets(std::vector<VkDescriptorSet> InSets)
    {
    	auto Device = FVulkanRenderContext::GetDevice();

    	vkFreeDescriptorSets(Device, DescriptorPool, InSets.size(), InSets.data());
    }

    void FVulkanRenderAPI::RenderMeshTasks(std::shared_ptr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data)
    {
    	FRenderer::Submit([&, Pipeline, Dimensions, Data]()
    	{
			std::shared_ptr<FVulkanPipeline> vk_pipeline = std::dynamic_pointer_cast<FVulkanPipeline>(Pipeline);
			
			if (Data.Size)
			{
				vkCmdPushConstants(CurrentCommandBuffer->GetCommandBuffer(), vk_pipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, Data.Size, Data.Data);
				delete[] Data.Data;
			}
			vkCmdBindPipeline(CurrentCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->GetPipeline());
			//vkCmdDrawMeshTasksEXT(CurrentCommandBuffer->GetCommandBuffer(), Dimensions.x, Dimensions.y, Dimensions.z);
		});
    }

    std::shared_ptr<FCommandBuffer> FVulkanRenderAPI::GetCommandBuffer()
    {
        return CurrentCommandBuffer;
    }

    void FVulkanRenderAPI::BeginCommandRecord()
    {
        FRenderer::Submit([&]
        {
            FRenderer::GetCommandBuffer()->Reset();
            FRenderer::GetCommandBuffer()->Begin();
        });
    }

    void FVulkanRenderAPI::EndCommandRecord()
    {
        FRenderer::Submit([&]
        {
            FRenderer::GetCommandBuffer()->End(); 
        });
    }

    void FVulkanRenderAPI::ExecuteCurrentCommands()
    {
        FRenderer::Submit([&]
        {
            VkPipelineStageFlags StageMasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            auto VkCmdBuffer = std::dynamic_pointer_cast<FVulkanCommandBuffer>(FRenderer::GetCommandBuffer());
            auto Semaphores = Swapchain->GetSemaphores();

            VkSubmitInfo SubmitInfo = {};
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = &VkCmdBuffer->GetCommandBuffer();
            SubmitInfo.signalSemaphoreCount = 1;
            SubmitInfo.pSignalSemaphores = &Semaphores.Render;
            SubmitInfo.waitSemaphoreCount = 1;
            SubmitInfo.pWaitSemaphores = &Semaphores.Present;
            SubmitInfo.pWaitDstStageMask = StageMasks;

            FVulkanRenderContext& Context = FVulkanRenderContext::Get();
            
            vkQueueSubmit(Context.GetGeneralQueue(), 1, &SubmitInfo, Swapchain->GetCurrentFence());
            
        });
    }

    void FVulkanRenderAPI::RenderImGui()
    {
    	auto image = Swapchain->GetCurrentImage();

    	BeginRender(
			{ image },
			image->GetSpecification().Extent,
			{ 0,0 },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);

    	FRenderer::Submit([&]
    	{
			ImGui::Render();
			ImDrawData* draw_data = ImGui::GetDrawData();
			ImGui_ImplVulkan_RenderDrawData(draw_data, CurrentCommandBuffer->GetCommandBuffer());
		});
    	
    	EndRender();
    }
}
