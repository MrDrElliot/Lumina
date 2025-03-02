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
        // Reset any images or buffers to their default desired layouts.
        CommandListTracker.ResetBufferDefaultStates();
        CommandListTracker.ResetImageDefaultStates();
        CommitBarriers();
        
        // Clear the recording state and end the command buffer. 
        PendingState.ClearPendingState(EPendingGraphicsState::Recording);
        VK_CHECK(vkEndCommandBuffer(CurrentCommandBuffer->CommandBuffer));
    }

    void FVulkanCommandList::Executed(FQueue* Queue)
    {
        CommandListTracker.CommandListExecuted(this);
    }

    void FVulkanCommandList::CopyImage(FRHIImage* Src, FRHIImage* Dst)
    {
        Assert(Src != nullptr && Dst != nullptr);

        CurrentCommandBuffer->AddReferencedResource(Src);
        CurrentCommandBuffer->AddReferencedResource(Dst);
        FVulkanImage* VulkanImageSrc = (FVulkanImage*)Src;
        FVulkanImage* VulkanImageDst = (FVulkanImage*)Dst;
        
        VkBlitImageInfo2 BlitInfo = {};
        BlitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
        BlitInfo.srcImage = Src->GetAPIResource<VkImage>(EAPIResourceType::Image);
        BlitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        BlitInfo.dstImage = Dst->GetAPIResource<VkImage>(EAPIResourceType::Image);
        BlitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        BlitInfo.filter = VK_FILTER_LINEAR;

        VkImageBlit2 BlitRegion = {};
        BlitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
    
        // Source Image
        BlitRegion.srcSubresource.aspectMask = VulkanImageSrc->GetFullAspectMask();
        BlitRegion.srcSubresource.mipLevel = 0;
        BlitRegion.srcSubresource.baseArrayLayer = 0;
        BlitRegion.srcSubresource.layerCount = 1;
        BlitRegion.srcOffsets[0] = { 0, 0, 0 };
        BlitRegion.srcOffsets[1] = { (int32)Src->GetSizeX(), (int32)Src->GetSizeY(), 1 };

        // Destination Image
        BlitRegion.dstSubresource.aspectMask = VulkanImageDst->GetFullAspectMask();
        BlitRegion.dstSubresource.mipLevel = 0;
        BlitRegion.dstSubresource.baseArrayLayer = 0;
        BlitRegion.dstSubresource.layerCount = 1;
        BlitRegion.dstOffsets[0] = { 0, 0, 0 };
        BlitRegion.dstOffsets[1] = { (int32)Dst->GetSizeX(), (int32)Dst->GetSizeY(), 1 };

        BlitInfo.regionCount = 1;
        BlitInfo.pRegions = &BlitRegion;

        SetRequiredImageAccess(Src, ERHIAccess::TransferRead);
        SetRequiredImageAccess(Dst, ERHIAccess::TransferWrite);
        CommitBarriers();

        vkCmdBlitImage2(CurrentCommandBuffer->CommandBuffer, &BlitInfo);

    }

    void FVulkanCommandList::WriteToImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, size_t RowPitch, size_t DepthPitch)
    {
        Assert(Dst != nullptr && Data != nullptr);

        CurrentCommandBuffer->AddReferencedResource(Dst);
        
        SetRequiredImageAccess(Dst, ERHIAccess::TransferWrite);
        CommitBarriers();
    
        TRefCountPtr<FVulkanBuffer> StagingBuffer = nullptr;
        RenderContext->GetStagingManager().GetStagingBuffer(StagingBuffer);
    
        Assert(StagingBuffer != nullptr);
    
        FVulkanMemoryAllocator* MemoryAllocator = RenderContext->GetDevice()->GetAllocator();
        VmaAllocation Allocation = MemoryAllocator->GetAllocation(StagingBuffer->GetBuffer());
        
        void* StagingData = MemoryAllocator->MapMemory(Allocation);
    
        size_t DataOffset = 0;
        for (uint32 y = 0; y < Dst->GetSizeY(); ++y)
        {
            memcpy((uint8*)StagingData + DataOffset, (const uint8*)Data + y * RowPitch, RowPitch);
            DataOffset += RowPitch;
        }
    
        MemoryAllocator->UnmapMemory(Allocation);

        FVulkanImage* VulkanImage = (FVulkanImage*)Dst;
        
        VkBufferImageCopy CopyRegion = {};
        CopyRegion.bufferOffset = 0;
        CopyRegion.bufferRowLength = 0;
        CopyRegion.bufferImageHeight = 0;
        CopyRegion.imageSubresource.aspectMask = VulkanImage->GetFullAspectMask();
        CopyRegion.imageSubresource.mipLevel = MipLevel;
        CopyRegion.imageSubresource.baseArrayLayer = ArraySlice;
        CopyRegion.imageSubresource.layerCount = 1;
        CopyRegion.imageOffset = { 0, 0, 0 };
        CopyRegion.imageExtent = { Dst->GetSizeX(), Dst->GetSizeY(), 1 };
    
        vkCmdCopyBufferToImage(
            CurrentCommandBuffer->CommandBuffer,
            StagingBuffer->GetBuffer(),
            Dst->GetAPIResource<VkImage>(EAPIResourceType::Image),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &CopyRegion
        );
    
        RenderContext->GetStagingManager().FreeStagingBuffer(StagingBuffer);
    }


    void FVulkanCommandList::CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize)
    {
        Assert(PendingState.IsRecording());
        Assert(Source != nullptr);
        Assert(Destination != nullptr);
        Assert(CopySize > 0);
        
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = SrcOffset;
        copyRegion.dstOffset = DstOffset;
        copyRegion.size = CopySize;

        FVulkanBuffer* VkSource = static_cast<FVulkanBuffer*>(Source);
        FVulkanBuffer* VkDestination = static_cast<FVulkanBuffer*>(Destination);
        
        vkCmdCopyBuffer(CurrentCommandBuffer->CommandBuffer, VkSource->GetBuffer(), VkDestination->GetBuffer(), 1, &copyRegion);

    }

    void FVulkanCommandList::UploadToBuffer(FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size)
    {
        Assert(Size > 0 && Size <= Buffer->GetSize());
        Assert(PendingState.IsRecording());
        
        CurrentCommandBuffer->AddReferencedResource(Buffer);
        
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

            CopyBuffer(StagingBuffer, 0, Buffer, 0, Size);

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
