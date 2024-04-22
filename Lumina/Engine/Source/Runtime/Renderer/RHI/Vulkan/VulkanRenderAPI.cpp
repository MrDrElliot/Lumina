#include "VulkanRenderAPI.h"

#include "imgui.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanPipeline.h"
#include "backends/imgui_impl_vulkan.h"
#include "Source/Runtime/Log/Log.h"

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
		VkRenderingAttachmentInfo depth_attachment = {};

			std::vector<VkRenderingAttachmentInfo> color_attachments = {};

			for (auto attachment : Attachments)
			{
				std::shared_ptr<FVulkanImage> vk_target = std::dynamic_pointer_cast<FVulkanImage>(attachment);
				FImageSpecification target_spec = vk_target->GetSpecification();

				if (target_spec.Usage == EImageUsage::RENDER_TARGET)
				{
					VkImageMemoryBarrier target_barrier = {};
					target_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					target_barrier.image = vk_target->GetImage();
					target_barrier.oldLayout = (VkImageLayout)vk_target->GetLayout();
					target_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					target_barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
					target_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					target_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					target_barrier.subresourceRange.baseArrayLayer = 0;
					target_barrier.subresourceRange.baseMipLevel = 0;
					target_barrier.subresourceRange.layerCount = 1;
					target_barrier.subresourceRange.levelCount = 1;

					vkCmdPipelineBarrier(CurrentCommandBuffer->GetCommandBuffer(),
						VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
						VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1, 
						&target_barrier
					);

					vk_target->SetCurrentLayout(EImageLayout::COLOR_ATTACHMENT);

					VkRenderingAttachmentInfo color_attachment = {};
					color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
					color_attachment.imageView = vk_target->GetImageView();
					color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					if (ClearColor.a != 0.0f)
					{
						color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					}
					else
					{
						color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
					}
					color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					color_attachment.clearValue = {{ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a}};

					color_attachments.push_back(color_attachment);
				}
				else {
					depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
					depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
					depth_attachment.imageView = vk_target->GetImageView();
					depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					depth_attachment.clearValue.color = {{0,0,0,1}};
					if (ClearColor.a != 0.0f)
					{
						depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					}
					else
					{
						depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
					}
					depth_attachment.clearValue.depthStencil = { 0.0f, 0 };
				}
			}
			

			VkRenderingInfo rendering_info = {};
			rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			rendering_info.renderArea = { { RenderOffset.x, RenderOffset.y }, { RenderArea.x, RenderArea.y } };
			rendering_info.layerCount = 1;
			rendering_info.colorAttachmentCount = color_attachments.size();
			rendering_info.pColorAttachments = color_attachments.data();
			rendering_info.pDepthAttachment = depth_attachment.imageView ? &depth_attachment : nullptr;
			rendering_info.pStencilAttachment = nullptr;



			VkRect2D scissor = { {0,0}, {RenderArea.x, RenderArea.y} };
			VkViewport viewport = { 0, (glm::float32)RenderArea.y, (glm::float32)RenderArea.x, -(glm::float32)RenderArea.y, 0.0f, 1.0f};
			vkCmdSetScissor(CurrentCommandBuffer->GetCommandBuffer(), 0, 1, &scissor);
			vkCmdSetViewport(CurrentCommandBuffer->GetCommandBuffer(), 0, 1, &viewport);
			vkCmdBeginRendering(CurrentCommandBuffer->GetCommandBuffer(), &rendering_info);
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

    std::shared_ptr<FImage> FVulkanRenderAPI::GetSwapchainImage()
    {
        return Swapchain->GetCurrentImage();
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
						
			vk_image->SetLayout(
				CurrentCommandBuffer,
				EImageLayout::TRANSFER_DST,
				EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
				EPipelineStage::TRANSFER,
				EPipelineAccess::COLOR_ATTACHMENT_WRITE,
				EPipelineAccess::TRANSFER_READ
			);
						
			vkCmdBlitImage(
				CurrentCommandBuffer->GetCommandBuffer(),
				vk_image->GetImage(),
				(VkImageLayout)vk_image->GetLayout(),
				std::dynamic_pointer_cast<FVulkanImage>(ImageToCopy)->GetImage(),
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

    void FVulkanRenderAPI::RenderMeshIndexed(std::shared_ptr<FBuffer> VertexBuffer, std::shared_ptr<FBuffer> IndexBuffer)
    {
		FRenderer::Submit([&, VertexBuffer, IndexBuffer]
		{
			VkCommandBuffer Buffer = CurrentCommandBuffer->GetCommandBuffer();
			std::shared_ptr<FVulkanBuffer> VkVertexBuffer = std::dynamic_pointer_cast<FVulkanBuffer>(VertexBuffer);
			std::shared_ptr<FVulkanBuffer> VkIndexBuffer= std::dynamic_pointer_cast<FVulkanBuffer>(IndexBuffer);
			
			VkBuffer BindBuffer = VkVertexBuffer->GetBuffer();
			VkDeviceSize Size = 0;
			
			vkCmdBindVertexBuffers(Buffer, 0, 1, &BindBuffer, &Size);
			vkCmdBindIndexBuffer(Buffer, VkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(Buffer, 0, 1, 0, 0, 0);
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
			
			delete[] Data.Data;
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
