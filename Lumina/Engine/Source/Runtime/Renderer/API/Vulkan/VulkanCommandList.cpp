#include "VulkanCommandList.h"
#include "VulkanBarriers.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "VulkanResources.h"
#include "VulkanSwapchain.h"
#include "Core/Profiler/Profile.h"
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
        LUMINA_PROFILE_SCOPE();
        CurrentCommandBuffer = RenderContext->GetQueue(Info.CommandQueue)->GetOrCreateCommandBuffer();
        
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        BeginInfo.pInheritanceInfo = nullptr;
        
        VK_CHECK(vkBeginCommandBuffer(CurrentCommandBuffer->CommandBuffer, &BeginInfo));
        
        PendingState.AddPendingState(EPendingCommandState::Recording);
    }

    void FVulkanCommandList::Close()
    {
        LUMINA_PROFILE_SCOPE();
        if (Info.CommandQueue != ECommandQueue::Transfer)
        {
            TracyVkCollect(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer)
        }

        // Reset any images or buffers to their default desired layouts.
        CommandListTracker.ResetBufferDefaultStates();
        CommandListTracker.ResetImageDefaultStates();
        CommitBarriers();
        
        // Clear the recording state and end the command buffer. 
        PendingState.ClearPendingState(EPendingCommandState::Recording);
        VK_CHECK(vkEndCommandBuffer(CurrentCommandBuffer->CommandBuffer));
    }

    void FVulkanCommandList::Executed(FQueue* Queue)
    {
        LUMINA_PROFILE_SCOPE();
        CommandListTracker.CommandListExecuted(this);
        
        PushConstantVisibility =    VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        CurrentPipelineLayout =     VK_NULL_HANDLE;
        ComputeState =           {};
        GraphicsState =          {};
    }

    void FVulkanCommandList::CopyImage(FRHIImage* Src, FRHIImage* Dst)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(Src != nullptr && Dst != nullptr)
        
        CurrentCommandBuffer->AddReferencedResource(Src);
        CurrentCommandBuffer->AddReferencedResource(Dst);

        FVulkanImage* VulkanImageSrc = (FVulkanImage*)Src;
        FVulkanImage* VulkanImageDst = (FVulkanImage*)Dst;
        
        VkBlitImageInfo2 BlitInfo = {};
        BlitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
        BlitInfo.srcImage = Src->GetAPIResource<VkImage>();
        BlitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        BlitInfo.dstImage = Dst->GetAPIResource<VkImage>();
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

    void FVulkanCommandList::WriteToImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, SIZE_T RowPitch, SIZE_T DepthPitch)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(Dst != nullptr && Data != nullptr)

        CurrentCommandBuffer->AddReferencedResource(Dst);
        
        SetRequiredImageAccess(Dst, ERHIAccess::TransferWrite);
        CommitBarriers();
    
        TRefCountPtr<FVulkanBuffer> StagingBuffer = nullptr;
        RenderContext->GetStagingManager().GetStagingBuffer(StagingBuffer);

        Assert(StagingBuffer != nullptr)
        Assert(Dst->GetSizeY() * RowPitch <= StagingBuffer->GetSize());
    
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
            Dst->GetAPIResource<VkImage, EAPIResourceType::Image>(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &CopyRegion
        );
    }


    void FVulkanCommandList::CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(PendingState.IsRecording())
        Assert(Source != nullptr)
        Assert(Destination != nullptr)
        Assert(CopySize > 0)

        CurrentCommandBuffer->AddReferencedResource(Source);
        CurrentCommandBuffer->AddReferencedResource(Destination);
        
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = SrcOffset;
        copyRegion.dstOffset = DstOffset;
        copyRegion.size = CopySize;

        FVulkanBuffer* VkSource = static_cast<FVulkanBuffer*>(Source);
        FVulkanBuffer* VkDestination = static_cast<FVulkanBuffer*>(Destination);
        
        vkCmdCopyBuffer(CurrentCommandBuffer->CommandBuffer, VkSource->GetBuffer(), VkDestination->GetBuffer(), 1, &copyRegion);

    }

    void FVulkanCommandList::UploadToBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Offset, uint32 Size)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(Size > 0 && Size <= Buffer->GetSize())
        
        CurrentCommandBuffer->AddReferencedResource(Buffer);
        
        if(Buffer->GetDescription().Usage.IsFlagCleared(EBufferUsageFlags::CPUWritable))
        {
            TRefCountPtr<FVulkanBuffer> StagingBuffer;
            RenderContext->GetStagingManager().GetStagingBuffer(StagingBuffer);
            Assert(StagingBuffer != nullptr)

            FVulkanMemoryAllocator* MemoryAllocator = RenderContext->GetDevice()->GetAllocator();
            VmaAllocation Allocation = MemoryAllocator->GetAllocation(StagingBuffer->GetBuffer());
            void* SourceData = const_cast<void*>(Data);
            void* StagingData = MemoryAllocator->MapMemory(Allocation);
            Memory::Memcpy(StagingData, SourceData, Size);
            MemoryAllocator->UnmapMemory(Allocation);

            CopyBuffer(StagingBuffer, 0, Buffer, 0, Size);

            //RenderContext->GetStagingManager().FreeStagingBuffer(StagingBuffer);
        }
        else
        {
            LOG_ERROR("Using UploadToBuffer on a mappable buffer is invalid.");
        }
    }

    void FVulkanCommandList::SetRequiredImageAccess(FRHIImage* Image, ERHIAccess Access)
    {
        LUMINA_PROFILE_SCOPE();

        CommandListTracker.RequireImageAccess(Image, Access);

        if (CurrentCommandBuffer)
        {
            CurrentCommandBuffer->AddReferencedResource(Image);
        }
    }

    void FVulkanCommandList::SetRequiredBufferAccess(FRHIBuffer* Buffer, ERHIAccess Access)
    {
        LUMINA_PROFILE_SCOPE();

        CommandListTracker.RequireBufferAccess(Buffer, Access);

        if (CurrentCommandBuffer)
        {
            CurrentCommandBuffer->AddReferencedResource(Buffer);
        }
    }

    void FVulkanCommandList::CommitBarriers()
    {
        LUMINA_PROFILE_SCOPE();

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

    void FVulkanCommandList::SetResourceStatesForBindingSet(FRHIBindingSet* BindingSet)
    {
        LUMINA_PROFILE_SCOPE();

        FVulkanBindingSet* VkBindingSet = static_cast<FVulkanBindingSet*>(BindingSet);

        for (uint32 Binding : VkBindingSet->BindingsRequiringTransitions)
        {
            const FBindingSetItem& Item = VkBindingSet->Desc.Bindings[Binding];

            switch (Item.Type)
            {
                case ERHIBindingResourceType::Texture_SRV:
                    {
                        FVulkanImage* VulkanImage = static_cast<FVulkanImage*>(Item.ResourceHandle);
                        SetRequiredImageAccess(VulkanImage, ERHIAccess::ShaderRead);
                    }
                    break;
                case ERHIBindingResourceType::Texture_UAV:
                    {
                        FVulkanImage* VulkanImage = static_cast<FVulkanImage*>(Item.ResourceHandle);
                        SetRequiredImageAccess(VulkanImage, ERHIAccess::ShaderWrite);
                    }
                    break;
                case ERHIBindingResourceType::Buffer_SRV:
                    break;
                case ERHIBindingResourceType::Buffer_UAV:
                    break;
                case ERHIBindingResourceType::Buffer_CBV:
                    break;
            }
        }
    }
    
    void FVulkanCommandList::AddMarker(const char* Name, const FColor& Color)
    {

        if (PendingState.IsRecording())
        {
            
        }
    }

    void FVulkanCommandList::PopMarker()
    {

        if(PendingState.IsRecording())
        {
            
        }
    }

    void FVulkanCommandList::BeginRenderPass(const FRenderPassBeginInfo& PassInfo)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "BeginRenderPass")        

        TFixedVector<VkRenderingAttachmentInfo, 4> ColorAttachments(PassInfo.ColorAttachments.size());
        VkRenderingAttachmentInfo DepthAttachment = {};

        for (SIZE_T i = 0; i < PassInfo.ColorAttachments.size(); ++i)
        {
            FRHIImageRef Image = PassInfo.ColorAttachments[i];
            CurrentCommandBuffer->AddReferencedResource(Image);

            TRefCountPtr<FVulkanImage> VulkanImage = Image.As<FVulkanImage>();

            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.ColorLoadOps[i] == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.ColorStoreOps[i] == ERenderStoreOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE; 

            if (!PassInfo.ClearColorValues.empty())
            {
                Attachment.clearValue.color.float32[0] = PassInfo.ClearColorValues[i].R;
                Attachment.clearValue.color.float32[1] = PassInfo.ClearColorValues[i].G;
                Attachment.clearValue.color.float32[2] = PassInfo.ClearColorValues[i].B;
                Attachment.clearValue.color.float32[3] = PassInfo.ClearColorValues[i].A;
            }
            
            SetRequiredImageAccess(Image, ERHIAccess::ColorAttachmentWrite);
            ColorAttachments[i] = Attachment;
        }
        
        CommitBarriers();
        
        const FRHIImageRef& ImageHandle = PassInfo.DepthAttachment;
        if (ImageHandle.IsValid())
        {
            TRefCountPtr<FVulkanImage> VulkanImage = PassInfo.DepthAttachment.As<FVulkanImage>();
            CurrentCommandBuffer->AddReferencedResource(VulkanImage);
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.DepthLoadOp == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = PassInfo.DepthStoreOp == ERenderStoreOp::DontCare ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
            
            DepthAttachment = Attachment;
            DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            DepthAttachment.clearValue.depthStencil.depth = PassInfo.ClearDepth;
            DepthAttachment.clearValue.depthStencil.stencil = PassInfo.ClearStencil;
            
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

        GraphicsState.CurrentRenderPassInfo = PassInfo;
    }

    void FVulkanCommandList::EndRenderPass()
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "EndRenderPass")        

        vkCmdEndRendering(CurrentCommandBuffer->CommandBuffer);
        GraphicsState.CurrentRenderPassInfo = {};
    }

    void FVulkanCommandList::ClearImageColor(FRHIImage* Image, const FColor& Color)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "ClearImageColor")        
        Assert(Image != nullptr)
        
        CurrentCommandBuffer->AddReferencedResource(Image);
        
        SetRequiredImageAccess(Image, ERHIAccess::HostWrite);
        CommitBarriers();
        
        VkClearColorValue Value;
        Value.float32[0] = Color.R;
        Value.float32[1] = Color.G;
        Value.float32[2] = Color.B;
        Value.float32[3] = Color.A;

        VkImageSubresourceRange Range;
        Range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;   // Clearing the color aspect of the image
        Range.baseMipLevel   = 0;                           // First mip level
        Range.levelCount     = 1;                           // Only clearing one mip level
        Range.baseArrayLayer = 0;                           // First layer in the image
        Range.layerCount     = 1;                           // Only clearing one layer
        
        vkCmdClearColorImage(CurrentCommandBuffer->CommandBuffer, Image->GetAPIResource<VkImage, EAPIResourceType::Image>(), VK_IMAGE_LAYOUT_GENERAL, &Value, 1, &Range);
    }

    void FVulkanCommandList::BindBindingSets(TVector<FRHIBindingSet*> BindingSets, ERHIBindingPoint BindPoint)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "BindBindingSet")        
        
        VkPipelineBindPoint BindingPoint = (BindPoint == ERHIBindingPoint::Graphics) ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;

        CurrentPipelineLayout = (BindPoint == ERHIBindingPoint::Graphics) ? GraphicsState.Pipeline->PipelineLayout : ComputeState.Pipeline->PipelineLayout;
        PushConstantVisibility = (BindPoint == ERHIBindingPoint::Graphics) ? GraphicsState.Pipeline->PushConstantVisibility : ComputeState.Pipeline->PushConstantVisibility;

        TVector<VkDescriptorSet> DescriptorSets;
        for (SIZE_T i = 0; i < BindingSets.size(); ++i)
        {
            FVulkanBindingSet* VkBindingSet = static_cast<FVulkanBindingSet*>(BindingSets[i]);
            CurrentCommandBuffer->AddReferencedResource(VkBindingSet);

            DescriptorSets.push_back(VkBindingSet->DescriptorSet);
            
            SetResourceStatesForBindingSet(VkBindingSet);
            CommitBarriers();
        }
        
        vkCmdBindDescriptorSets(CurrentCommandBuffer->CommandBuffer, BindingPoint,
            CurrentPipelineLayout, 0, (uint32)DescriptorSets.size(), DescriptorSets.data(), 0, nullptr);
    }

    void FVulkanCommandList::SetPushConstants(const void* Data, SIZE_T ByteSize)
    {
        LUMINA_PROFILE_SCOPE();
        vkCmdPushConstants(CurrentCommandBuffer->CommandBuffer, CurrentPipelineLayout, PushConstantVisibility, 0, (uint32)ByteSize, Data);
    }

    void FVulkanCommandList::BindVertexBuffer(FRHIBuffer* Buffer, uint32 Index, uint32 Offset)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "BindVertexBuffer")        

        GraphicsState.SetVertexStream(Buffer, Index, Offset);
    }

    void FVulkanCommandList::SetGraphicsPipeline(FRHIGraphicsPipeline* InPipeline)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "SetGraphicsPipeline")        

        FVulkanGraphicsPipeline* PSO = static_cast<FVulkanGraphicsPipeline*>(InPipeline);

        if (GraphicsState.Pipeline != InPipeline)
        {
            PSO->Bind(CurrentCommandBuffer->CommandBuffer);
            CurrentCommandBuffer->AddReferencedResource(InPipeline);
            GraphicsState.Pipeline = PSO;
        }
    }

    void FVulkanCommandList::SetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "SetViewport")        

        VkViewport Viewport = {};
        Viewport.x        = MinX;
        Viewport.y        = MaxY; // <- Y offset at bottom
        Viewport.width    = MaxX - MinX;
        Viewport.height   = -(MaxY - MinY); // <- Negative height
        Viewport.minDepth = MinZ;
        Viewport.maxDepth = MaxZ;

        vkCmdSetViewport(CurrentCommandBuffer->CommandBuffer, 0, 1, &Viewport);
    }
    
    void FVulkanCommandList::SetScissorRect(uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "SetScissorRect")        

        VkRect2D Scissor = {};
        Scissor.offset.x = (int32)MinX;
        Scissor.offset.y = (int32)MinY;
        Scissor.extent.width = MaxX - MinX;
        Scissor.extent.height = MaxY - MinY;

        vkCmdSetScissor(CurrentCommandBuffer->CommandBuffer, 0, 1, &Scissor);
    }
    
    void FVulkanCommandList::Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "Draw")        
        Assert(GraphicsState.Pipeline != nullptr)

        GraphicsState.PrepareForDraw(CurrentCommandBuffer->CommandBuffer);
        vkCmdDraw(CurrentCommandBuffer->CommandBuffer, VertexCount, InstanceCount, FirstVertex, FirstInstance);
    }

    void FVulkanCommandList::DrawIndexed(FRHIBuffer* IndexBuffer, uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "DrawIndexed")        
        Assert(GraphicsState.Pipeline != nullptr)
        Assert(IndexBuffer != nullptr)
        Assert(IndexBuffer->GetUsage().IsFlagSet(EBufferUsageFlags::IndexBuffer))

        FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(IndexBuffer);
        CurrentCommandBuffer->AddReferencedResource(Buffer);

        GraphicsState.PrepareForDraw(CurrentCommandBuffer->CommandBuffer);
        vkCmdBindIndexBuffer(CurrentCommandBuffer->CommandBuffer, Buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(CurrentCommandBuffer->CommandBuffer, IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
    }

    void FVulkanCommandList::SetComputePipeline(FRHIComputePipeline* InPipeline)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "SetComputePipeline")        

        FVulkanComputePipeline* PSO = static_cast<FVulkanComputePipeline*>(InPipeline);

        if (ComputeState.Pipeline != InPipeline)
        {
            PSO->Bind(CurrentCommandBuffer->CommandBuffer);
            CurrentCommandBuffer->AddReferencedResource(InPipeline);
            ComputeState.Pipeline = PSO;
        }
        
    }

    void FVulkanCommandList::Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "Dispatch")        

        Assert(ComputeState.Pipeline != nullptr)
        vkCmdDispatch(CurrentCommandBuffer->CommandBuffer, GroupCountX, GroupCountY, GroupCountZ);
    }

    void* FVulkanCommandList::GetAPIResourceImpl(EAPIResourceType Type)
    {
        return CurrentCommandBuffer->CommandBuffer;
    }
}
