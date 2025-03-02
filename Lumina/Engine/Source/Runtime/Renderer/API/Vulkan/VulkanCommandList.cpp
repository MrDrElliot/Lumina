#include "VulkanCommandList.h"

#include "VulkanBarriers.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "VulkanResources.h"
#include "VulkanSwapchain.h"
#include "Renderer/RenderContext.h"

namespace Lumina
{
    
    VkImageLayout ConvertRHIAccessToVkImageLayout(ERHIAccess Access)
    {
        switch (Access)
        {
        case ERHIAccess::TransferRead: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            
        case ERHIAccess::TransferWrite: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            
        case ERHIAccess::ShaderWrite: return VK_IMAGE_LAYOUT_GENERAL;
            
        case ERHIAccess::ShaderRead: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            
        case ERHIAccess::ColorAttachmentWrite: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            
        case ERHIAccess::DepthStencilAttachmentWrite: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            
        case ERHIAccess::PresentRead: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            
        case ERHIAccess::ComputeWrite: return VK_IMAGE_LAYOUT_GENERAL;
            
        case ERHIAccess::ComputeRead: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            
        case ERHIAccess::HostRead: return VK_IMAGE_LAYOUT_GENERAL;
        case ERHIAccess::HostWrite: return VK_IMAGE_LAYOUT_GENERAL;
        case ERHIAccess::All: return VK_IMAGE_LAYOUT_GENERAL;
    
        default: return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }



    void FVulkanCommandList::Open()
    {
        CurrentCommandBuffer = RenderContext->GetQueue(Info.CommandQueue)->GetOrCreateCommandBuffer();
        
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        BeginInfo.pInheritanceInfo = nullptr;
        
        VK_CHECK(vkBeginCommandBuffer(CurrentCommandBuffer->CommandBuffer, &BeginInfo));

        PendingState.AddPendingState(EPendingGraphicsState::Recording);
    }

    void FVulkanCommandList::Close()
    {
        CommandListTracker.ResetBufferDefaultStates();
        CommandListTracker.ResetImageDefaultStates();
        CommitBarriers();
        
        
        PendingState.ClearPendingState(EPendingGraphicsState::Recording);
        VK_CHECK(vkEndCommandBuffer(CurrentCommandBuffer->CommandBuffer));
    }

    void FVulkanCommandList::Executed(FQueue* Queue)
    {
        CommandListTracker.CommandListExecuted(this);
    }

    void FVulkanCommandList::CopyBuffer(FRHIBuffer* Source, FRHIBuffer* Destination)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = CurrentCommandBuffer->CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
       // VK_CHECK(vkAllocateCommandBuffers(, &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = Source->GetSize();

        FVulkanBuffer* VkSource = static_cast<FVulkanBuffer*>(Source);
        FVulkanBuffer* VkDestination = static_cast<FVulkanBuffer*>(Destination);
        
        vkCmdCopyBuffer(commandBuffer, VkSource->GetBuffer(), VkDestination->GetBuffer(), 1, &copyRegion);

        VK_CHECK(vkEndCommandBuffer(commandBuffer));
    }

    void FVulkanCommandList::UploadToBuffer(FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size)
    {
        Assert(Size <= Buffer->GetSize());
        Assert(PendingState.IsRecording());
        
        CurrentCommandBuffer->ReferencedResources.push_back(Buffer);
        
        if(Buffer->GetDescription().Usage.IsFlagCleared(EBufferUsageFlags::CPUWritable))
        {
            TRefCountPtr<FVulkanBuffer> StagingBuffer;
            RenderContext->GetStagingManager().GetStagingBuffer(StagingBuffer);
            Assert(StagingBuffer != nullptr);

            FVulkanMemoryAllocator* MemoryAllocator = RenderContext->GetDevice()->GetAllocator();
            VmaAllocation Allocation = MemoryAllocator->GetAllocation(StagingBuffer->GetBuffer());
            void* StagingData = MemoryAllocator->MapMemory(Allocation);
            FMemory::MemCopy(StagingData, Data, Size);
            MemoryAllocator->UnmapMemory(Allocation);

            RenderContext->GetStagingManager().FreeStagingBuffer(StagingBuffer);
            
        }
        else
        {
            LOG_ERROR("Using UploadToBuffer on a mappable buffer is invalid.");
        }
    }

    void FVulkanCommandList::SetRequiredImageAccess(FRHIImageRef Image, ERHIAccess Access)
    {
        CommandListTracker.RequireImageAccess(Image, Access);
    }

    void FVulkanCommandList::CommitBarriers()
    {
        FVulkanPipelineBarrier Barrier;

        for (const FImageBarrier& ImageBarrier : CommandListTracker.GetImageBarriers())
        {
            Barrier.AddFullImageLayoutTransition(*ImageBarrier.Image.As<FVulkanImage>(),
                ConvertRHIAccessToVkImageLayout(ImageBarrier.AccessBefore),
                ConvertRHIAccessToVkImageLayout(ImageBarrier.AccessAfter));            
        }
        
        Barrier.Execute(this);

        CommandListTracker.ClearBarriers();
    }

    void FVulkanCommandList::AddMarker(const char* Name)
    {
        if (PendingState.IsRecording())
            {
            VkDebugMarkerMarkerInfoEXT markerInfo = {};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
            markerInfo.pMarkerName = Name;

            RenderContext->GetDebugUtils().vkCmdDebugMarkerBeginEXT(CurrentCommandBuffer->CommandBuffer, &markerInfo);
        }
    }

    void FVulkanCommandList::PopMarker()
    {
        if(PendingState.IsRecording())
        {
            RenderContext->GetDebugUtils().vkCmdDebugMarkerEndEXT(CurrentCommandBuffer->CommandBuffer);
        }
    }

    void FVulkanCommandList::BeginRenderPass(const FRenderPassBeginInfo& PassInfo)
    {
        PendingState.AddPendingState(EPendingGraphicsState::RenderPass);

        
        TVector<VkRenderingAttachmentInfo> ColorAttachments;
        VkRenderingAttachmentInfo DepthAttachment = {};

        for (int i = 0; i < PassInfo.ColorAttachments.size(); ++i)
        {
            FRHIImageRef Image = PassInfo.ColorAttachments[i];

            TRefCountPtr<FVulkanImage> VulkanImage = Image.As<FVulkanImage>();
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.ColorLoadOps[i] == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.ColorStoreOps[i] == ERenderLoadOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE; 
    
            Attachment.clearValue.color.float32[0] = PassInfo.ClearColorValues[i].R;
            Attachment.clearValue.color.float32[1] = PassInfo.ClearColorValues[i].G;
            Attachment.clearValue.color.float32[2] = PassInfo.ClearColorValues[i].B;
            Attachment.clearValue.color.float32[3] = PassInfo.ClearColorValues[i].A;
    
            ColorAttachments.push_back(Attachment);
        }
        
        const FRHIImageRef& ImageHandle = PassInfo.DepthAttachment;
        if (ImageHandle.IsValid())
        {
            TRefCountPtr<FVulkanImage> VulkanImage = PassInfo.DepthAttachment.As<FVulkanImage>();
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.DepthLoadOp == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.DepthStoreOp == ERenderLoadOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
            
            DepthAttachment = Attachment;
            DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            DepthAttachment.clearValue.depthStencil.depth = 1.0f;
            DepthAttachment.clearValue.depthStencil.stencil = 0;
            
        }
        
        VkRenderingInfo RenderInfo = {};
        RenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        RenderInfo.colorAttachmentCount = (uint32)ColorAttachments.size();
        RenderInfo.pColorAttachments = ColorAttachments.data();
        RenderInfo.pDepthAttachment = (DepthAttachment.imageView != VK_NULL_HANDLE) ? &DepthAttachment : nullptr;
        RenderInfo.renderArea.extent.width = PassInfo.RenderArea.X;
        RenderInfo.renderArea.extent.height = PassInfo.RenderArea.Y;
        RenderInfo.layerCount = 1;
        
        vkCmdBeginRendering(CurrentCommandBuffer->CommandBuffer, &RenderInfo);
    }

    void FVulkanCommandList::EndRenderPass()
    {
        vkCmdEndRendering(CurrentCommandBuffer->CommandBuffer);
        PendingState.ClearPendingState(EPendingGraphicsState::RenderPass);

    }

    void FVulkanCommandList::ClearColor(const FColor& Color)
    {
        TRefCountPtr<FVulkanImage> Image = RenderContext->GetSwapchain()->GetCurrentImage();

        SetRequiredImageAccess(Image, ERHIAccess::HostWrite);
        CommitBarriers();
        
        VkClearColorValue Value;
        Value.float32[0] = Color.R;
        Value.float32[1] = Color.G;
        Value.float32[2] = Color.B;
        Value.float32[3] = Color.A;

        VkImageSubresourceRange Range;
        Range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT; // Clearing the color aspect of the image
        Range.baseMipLevel   = 0; // First mip level
        Range.levelCount     = 1; // Only clearing one mip level
        Range.baseArrayLayer = 0; // First layer in the image
        Range.layerCount     = 1; // Only clearing one layer
        
        vkCmdClearColorImage(CurrentCommandBuffer->CommandBuffer, Image->GetImage(), VK_IMAGE_LAYOUT_GENERAL, &Value, 1, &Range);
        
    }

    void FVulkanCommandList::Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance)
    {
        vkCmdDraw(CurrentCommandBuffer->CommandBuffer, VertexCount, InstanceCount, FirstVertex, FirstInstance);
    }

    void FVulkanCommandList::DrawIndexed(uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance)
    {
        vkCmdDrawIndexed(CurrentCommandBuffer->CommandBuffer, IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
    }

    void FVulkanCommandList::Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ)
    {
        vkCmdDispatch(CurrentCommandBuffer->CommandBuffer, GroupCountX, GroupCountY, GroupCountZ);
    }

    void* FVulkanCommandList::GetAPIResourceImpl(EAPIResourceType Type)
    {
        return CurrentCommandBuffer->CommandBuffer;
    }
}
