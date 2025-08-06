#include "VulkanCommandList.h"

#include "Convert.h"
#include "VulkanBarriers.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "VulkanResources.h"
#include "VulkanSwapchain.h"
#include "Core/Profiler/Profile.h"
#include "Renderer/RenderContext.h"

namespace Lumina
{

    VkImageAspectFlags GuessImageAspectFlags(VkFormat format)
    {
        switch (format)
        {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case VK_FORMAT_S8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }
    
    void FVulkanCommandList::Open()
    {
        LUMINA_PROFILE_SCOPE();
        CurrentCommandBuffer = RenderContext->GetQueue(Info.CommandQueue)->GetOrCreateCommandBuffer();
        
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        VK_CHECK(vkBeginCommandBuffer(CurrentCommandBuffer->CommandBuffer, &BeginInfo));
        CurrentCommandBuffer->ReferencedResources.push_back(this);
        
        PendingState.AddPendingState(EPendingCommandState::Recording);
    }

    void FVulkanCommandList::Close()
    {
        LUMINA_PROFILE_SCOPE();
        
        EndRenderPass();
        
        if (Info.CommandQueue != ECommandQueue::Transfer)
        {
            TracyVkCollect(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer)
        }

        // Reset any images or buffers to their default desired layouts.
        StateTracker.KeepBufferInitialStates();
        StateTracker.KeepTextureInitialStates();
        CommitBarriers();
        
        // Clear the recording state and end the command buffer. 
        VK_CHECK(vkEndCommandBuffer(CurrentCommandBuffer->CommandBuffer));
        
        PendingState.ClearPendingState(EPendingCommandState::Recording);

        PushConstantVisibility =    VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        CurrentPipelineLayout =     VK_NULL_HANDLE;
        ComputeState =           {};
        GraphicsState =          {};
        bHasDynamicBufferWrites =   false;
        
        FlushDynamicBufferWrites();
    }

    void FVulkanCommandList::Executed(FQueue* Queue, uint64 SubmissionID)
    {
        LUMINA_PROFILE_SCOPE();

        CurrentCommandBuffer->SubmissionID = SubmissionID;
        uint64 RecordingID = CurrentCommandBuffer->RecordingID;
        
        CurrentCommandBuffer = nullptr;

        SubmitDynamicBuffers(RecordingID, SubmissionID);
        
        StateTracker.CommandListSubmitted();

        UploadManager->SubmitChunks(
            MakeVersion(RecordingID, Queue->Type, false),
            MakeVersion(SubmissionID, Queue->Type, true));

        DynamicBufferWrites.clear();
    }

    void FVulkanCommandList::CopyImage(FRHIImage* Src, const FTextureSlice& SrcSlice, FRHIImage* Dst, const FTextureSlice& DstSlice)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(Src != nullptr && Dst != nullptr)
        
        CurrentCommandBuffer->AddReferencedResource(Src);
        CurrentCommandBuffer->AddReferencedResource(Dst);

        auto ResolvedDstSlice = DstSlice.Resolve(Dst->DescRef);
        auto ResolvedSrcSlice = SrcSlice.Resolve(Src->DescRef);

        RequireTextureState(Src, FTextureSubresourceSet(ResolvedSrcSlice.MipLevel, 1, ResolvedSrcSlice.ArraySlice, 1), EResourceStates::CopySource);
        RequireTextureState(Dst, FTextureSubresourceSet(ResolvedDstSlice.MipLevel, 1, ResolvedDstSlice.ArraySlice, 1), EResourceStates::CopyDest);
        CommitBarriers();
        
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
        BlitRegion.srcSubresource.mipLevel = SrcSlice.MipLevel;
        BlitRegion.srcSubresource.baseArrayLayer = SrcSlice.ArraySlice;
        BlitRegion.srcSubresource.layerCount = 1;
        BlitRegion.srcOffsets[0] = { 0, 0, 0 };
        BlitRegion.srcOffsets[1] = { (int32)Src->GetSizeX(), (int32)Src->GetSizeY(), 1 };

        // Destination Image
        BlitRegion.dstSubresource.aspectMask = VulkanImageDst->GetFullAspectMask();
        BlitRegion.dstSubresource.mipLevel = DstSlice.MipLevel;
        BlitRegion.dstSubresource.baseArrayLayer = DstSlice.ArraySlice;
        BlitRegion.dstSubresource.layerCount = 1;
        BlitRegion.dstOffsets[0] = { 0, 0, 0 };
        BlitRegion.dstOffsets[1] = { (int32)Dst->GetSizeX(), (int32)Dst->GetSizeY(), 1 };

        BlitInfo.regionCount = 1;
        BlitInfo.pRegions = &BlitRegion;

        vkCmdBlitImage2(CurrentCommandBuffer->CommandBuffer, &BlitInfo);

    }

    static void ComputeMipLevelInformation(const FRHIImageDesc& Desc, uint32 MipLevel, uint32* WidthOut, uint32* HeightOut, uint32* DepthOut)
    {
        uint32 Width = std::max((uint32)Desc.Extent.X >> MipLevel, uint32(1));
        uint32 Height = std::max((uint32)Desc.Extent.Y >> MipLevel, uint32(1));
        uint32 Depth = std::max((uint32)Desc.Depth >> MipLevel, uint32(1));

        if (WidthOut)
        {
            *WidthOut = Width;
        }
        if (HeightOut)
        {
            *HeightOut = Height;
        }
        if (DepthOut)
        {
            *DepthOut = Depth;
        }
    }

    void FVulkanCommandList::WriteImage(FRHIImage* Dst, uint32 ArraySlice, uint32 MipLevel, const void* Data, SIZE_T RowPitch, SIZE_T DepthPitch)
    {
        LUMINA_PROFILE_SCOPE();
        Assert(Dst != nullptr && Data != nullptr)
        
        if (Dst->GetDescription().Extent.Y > 1 && RowPitch == 0)
        {
            LOG_ERROR("WriteImage: RowPitch is 0 but dest has multiple rows");
        }
        
        uint32 MipWidth, MipHeight, MipDepth;
        ComputeMipLevelInformation(Dst->GetDescription(), MipLevel, &MipWidth, &MipHeight, &MipDepth);

        const FFormatInfo& FormatInfo = GetFormatInfo(Dst->GetDescription().Format);
        uint32 DeviceNumCols = (MipWidth + FormatInfo.BlockSize - 1) / FormatInfo.BlockSize;
        uint32 DeviceNumRows = (MipHeight + FormatInfo.BlockSize - 1) / FormatInfo.BlockSize;
        uint32 DeviceRowPitch = DeviceNumCols * FormatInfo.BytesPerBlock;
        uint64 DeviceMemSize = uint64(DeviceRowPitch) * uint64(DeviceNumRows) * MipDepth;
        
        FRHIBuffer* UploadBuffer;
        uint64 UploadOffset;
        void* UploadCPUVA;
        UploadManager->SuballocateBuffer(DeviceMemSize, &UploadBuffer, &UploadOffset, &UploadCPUVA, MakeVersion(CurrentCommandBuffer->RecordingID, Info.CommandQueue, false));

        SIZE_T MinRowPitch = std::min(SIZE_T(DeviceRowPitch), RowPitch);
        uint8* MappedPtr = (uint8*)UploadCPUVA;
        for (uint32 Slice = 0; Slice < MipDepth; ++Slice)
        {
            const uint8* SourcePtr = (const uint8*)Data + DepthPitch * Slice;
            for (uint32 row = 0; row < DeviceNumRows; row++)
            {
                Memory::Memcpy(MappedPtr, SourcePtr, MinRowPitch);
                MappedPtr += DeviceRowPitch;
                SourcePtr += RowPitch;
            }
        }
        
        FVulkanImage* VulkanImage = (FVulkanImage*)Dst;
        
        VkBufferImageCopy CopyRegion = {};
        CopyRegion.bufferOffset = UploadOffset;
        CopyRegion.bufferRowLength = DeviceNumCols * FormatInfo.BlockSize;
        CopyRegion.bufferImageHeight = DeviceNumRows * FormatInfo.BlockSize;
        CopyRegion.imageSubresource.aspectMask = VulkanImage->GetFullAspectMask();
        CopyRegion.imageSubresource.mipLevel = MipLevel;
        CopyRegion.imageSubresource.baseArrayLayer = ArraySlice;
        CopyRegion.imageSubresource.layerCount = 1;
        CopyRegion.imageOffset = { 0, 0, 0 };
        CopyRegion.imageExtent = { MipWidth, MipHeight, MipDepth };


        SetImageState(Dst, FTextureSubresourceSet(MipLevel, 1, ArraySlice, 1), EResourceStates::CopyDest);
        CommitBarriers();

        CurrentCommandBuffer->AddReferencedResource(Dst);
        
        vkCmdCopyBufferToImage(
            CurrentCommandBuffer->CommandBuffer,
            UploadBuffer->GetAPIResource<VkBuffer>(),
            Dst->GetAPIResource<VkImage, EAPIResourceType::Image>(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &CopyRegion
        );
    }

    void FVulkanCommandList::CopyBuffer(FRHIBuffer* Source, uint64 SrcOffset, FRHIBuffer* Destination, uint64 DstOffset, uint64 CopySize)
    {
        LUMINA_PROFILE_SCOPE();
        
        Assert(DstOffset + CopySize <= Destination->GetDescription().Size)
        Assert(SrcOffset + CopySize <= Source->GetDescription().Size)

        if (Destination->GetDescription().Usage.IsFlagSet(EBufferUsageFlags::CPUWritable))
        {
            CurrentCommandBuffer->AddStagingResource(Destination);
        }
        else
        {
            CurrentCommandBuffer->AddReferencedResource(Destination);
        }

        if (Source->GetDescription().Usage.IsFlagSet(EBufferUsageFlags::CPUWritable))
        {
            CurrentCommandBuffer->AddStagingResource(Source);
        }
        else
        {
            CurrentCommandBuffer->AddReferencedResource(Source);
        }

        RequireBufferState(Source, EResourceStates::CopySource);
        RequireBufferState(Destination, EResourceStates::CopyDest);
        CommitBarriers();
        
        VkBufferCopy copyRegion = {};
        copyRegion.size = CopySize;
        copyRegion.srcOffset = SrcOffset;
        copyRegion.dstOffset = DstOffset;

        FVulkanBuffer* VkSource = static_cast<FVulkanBuffer*>(Source);
        FVulkanBuffer* VkDestination = static_cast<FVulkanBuffer*>(Destination);
        
        vkCmdCopyBuffer(CurrentCommandBuffer->CommandBuffer, VkSource->GetBuffer(), VkDestination->GetBuffer(), 1, &copyRegion);

    }

    void FVulkanCommandList::WriteBuffer(FRHIBuffer* Buffer, const void* Data, SIZE_T Offset, SIZE_T Size)
    {
        LUMINA_PROFILE_SCOPE();
        
        if (Size == 0)
        {
            // For ease of use, we make trying to write a size of 0 technically a silent fail, so you can just blindly upload and not need to worry about it.
            return;
        }
        
        if (Size > Buffer->GetSize())
        {
            LOG_ERROR("Buffer: \"{}\" has size: [{}], but tried to write [{}]", Buffer->GetDescription().DebugName, Buffer->GetSize(), Size);
            return;    
        }
        
        constexpr size_t vkCmdUpdateBufferLimit = 65536;
        CurrentCommandBuffer->AddReferencedResource(Buffer);
        FVulkanBuffer* VulkanBuffer = static_cast<FVulkanBuffer*>(Buffer);
        
        if (Buffer->GetDescription().Usage.IsFlagSet(BUF_Dynamic))
        {
            Assert(Offset == 0);
            auto GetQueueFinishID = [this] (ECommandQueue Queue)-> uint64
            {
                return RenderContext->GetQueue(Queue)->LastFinishedID;
            };
            
            FDynamicBufferWrite& Write = DynamicBufferWrites[Buffer];

            if (!Write.bInitialized)
            {
                Write.MinVersion = Buffer->GetDescription().MaxVersions;
                Write.MaxVersion = -1;
                Write.bInitialized = true;
            }

            TArray<uint64, uint32(ECommandQueue::Num)> QueueCompletionValues =
            {
                GetQueueFinishID(ECommandQueue::Graphics),
                GetQueueFinishID(ECommandQueue::Compute),
                GetQueueFinishID(ECommandQueue::Transfer),
            };
            
            uint32 SearchStart = VulkanBuffer->VersionSearchStart;
            uint32 MaxVersions = Buffer->GetDescription().MaxVersions;
            uint32 Version = 0;
            
            uint64 OriginalVersionInfo = 0;

            while (true)
            {
                bool bFound = false;
                
                for (SIZE_T i = 0; i < MaxVersions; ++i)
                {
                
                    Version = i + SearchStart;
                    Version = (Version >= MaxVersions) ? (Version - MaxVersions) : Version;

                    OriginalVersionInfo = VulkanBuffer->VersionTracking[Version];
                    
                    if (OriginalVersionInfo == 0)
                    {
                        bFound = true;
                        break;
                    }

                    bool bSubmitted = (OriginalVersionInfo & GVersionSubmittedFlag) != 0;
                    uint32 QueueIndex = uint32(OriginalVersionInfo >> GVersionQueueShift) & GVersionQueueMask;
                    uint64 ID = OriginalVersionInfo & GVersionIDMask;

                    if (bSubmitted)
                    {
                        if (QueueIndex >= uint32(ECommandQueue::Num))
                        {
                            bFound = true;
                            break;
                        }

                        if (ID <= QueueCompletionValues[QueueIndex])
                        {
                            bFound = true;
                            break;
                        }
                    }
                }

                if (!bFound)
                {
                    LOG_ERROR("Dynamic Buffer [] has MaxVersions: {} - Which is insufficient", Buffer->GetDescription().MaxVersions);
                    return;
                }

                uint64_t NewVersionInfo = (uint64_t(Info.CommandQueue) << GVersionQueueShift) | (CurrentCommandBuffer->RecordingID);

                if (VulkanBuffer->VersionTracking[Version].compare_exchange_weak(OriginalVersionInfo, NewVersionInfo))
                {
                    break;
                }
            }

            VulkanBuffer->VersionSearchStart = (Version + 1 < MaxVersions) ? (Version + 1) : 0;

            Write.LatestVersion = Version;
            Write.MinVersion = Math::Min<int32>(Version, Write.MinVersion);
            Write.MaxVersion = Math::Max<int32>(Version, Write.MaxVersion);

            void* HostData = (uint8*)VulkanBuffer->GetMappedMemory() + Version * VulkanBuffer->GetDescription().Size;
            
            void* SourceData = const_cast<void*>(Data);
            Memory::Memcpy(HostData, SourceData, Size);

            bHasDynamicBufferWrites = true;
        }
        
        
        // Per Vulkan spec, vkCmdUpdateBuffer requires that the data size is smaller than or equal to 64 kB,
        // and that the offset and data size are a multiple of 4. We can't change the offset, but data size
        // is rounded up later.
        if (Size <= vkCmdUpdateBufferLimit && (Offset & 3) == 0)
        {

            SetBufferState(Buffer, EResourceStates::CopyDest);
            CommitBarriers();

            // Round up the write size to a multiple of 4
            const SIZE_T SizeToWrite = (Size + 3) & ~3ull;
            
            vkCmdUpdateBuffer(CurrentCommandBuffer->CommandBuffer, Buffer->GetAPIResource<VkBuffer>(), Offset, SizeToWrite, Data);
        }
        else
        {
            if(Buffer->GetDescription().Usage.IsFlagCleared(EBufferUsageFlags::CPUWritable))
            {
                FRHIBuffer* UploadBuffer;
                uint64 UploadOffset;
                void* UploadCPUVA;
                UploadManager->SuballocateBuffer(Size, &UploadBuffer, &UploadOffset, &UploadCPUVA, MakeVersion(CurrentCommandBuffer->RecordingID, Info.CommandQueue, false));

                Memory::Memcpy(UploadCPUVA, Data, Size);
                CopyBuffer(UploadBuffer, UploadOffset, Buffer, Offset, Size);
            }
            else
            {
                LOG_ERROR("Using UploadToBuffer on a mappable buffer is invalid.");
            }
        }
    }

    void FVulkanCommandList::FlushDynamicBufferWrites()
    {
        TVector<VmaAllocation> Allocations;
        TVector<VkDeviceSize> Offsets;
        TVector<VkDeviceSize> Sizes;
    
        for (auto& Pair : DynamicBufferWrites)
        {
            FVulkanBuffer* Buffer = Pair.first.As<FVulkanBuffer>();
            FDynamicBufferWrite& Write = Pair.second;

            if (Write.MaxVersion < Write.MinVersion || !Write.bInitialized)
            {
                continue;
            }

            uint64 NumVersions = Write.MaxVersion - Write.MinVersion + 1;
            VkDeviceSize Offset = Write.MinVersion * Buffer->GetDescription().Size;
            VkDeviceSize Size = NumVersions * Buffer->GetDescription().Size;

            VmaAllocation Allocation = Buffer->Allocation;
            if (!Allocation)
            {
                LOG_WARN("Attempted to flush a dynamic buffer with no valid VmaAllocation.");
                continue;
            }

            Allocations.push_back(Allocation);
            Offsets.push_back(Offset);
            Sizes.push_back(Size);
        }

        if (!Allocations.empty())
        {
            VK_CHECK(vmaFlushAllocations(
                RenderContext->GetDevice()->GetAllocator()->GetAllocator(),
                static_cast<uint32_t>(Allocations.size()),
                Allocations.data(),
                Offsets.data(),
                Sizes.data()
            ));
        }
    }

    void FVulkanCommandList::SubmitDynamicBuffers(uint64 RecordingID, uint64 SubmittedID)
    {
        uint64 StateToFind = (uint64(Info.CommandQueue) << GVersionQueueShift) | (RecordingID & GVersionIDMask);
        uint64 StateToReplace = (uint64(Info.CommandQueue) << GVersionQueueShift) | (SubmittedID & GVersionIDMask) | GVersionSubmittedFlag;

        for (auto& Pair : DynamicBufferWrites)
        {
            FRHIBufferRef Buffer = Pair.first;
            FDynamicBufferWrite& Write = Pair.second;

            if (!Write.bInitialized)
            {
                continue;
            }

            for (SIZE_T i = Write.MinVersion; i <= Write.MaxVersion; ++i)
            {
                uint64 Expected = StateToFind;
                Buffer.As<FVulkanBuffer>()->VersionTracking[i].compare_exchange_strong(Expected, StateToReplace);
            }
        }
    }

    void FVulkanCommandList::SetPermanentImageState(FRHIImage* Image, EResourceStates StateBits)
    {
        StateTracker.SetPermanentTextureState(Image, AllSubresources, StateBits);

        if (CurrentCommandBuffer)
        {
            CurrentCommandBuffer->ReferencedResources.push_back(Image);
        }
    }

    void FVulkanCommandList::SetPermanentBufferState(FRHIBuffer* Buffer, EResourceStates StateBits)
    {
        StateTracker.SetPermanentBufferState(Buffer, StateBits);
        
        if (CurrentCommandBuffer)
        {
            CurrentCommandBuffer->ReferencedResources.push_back(Buffer);
        }
    }

    void FVulkanCommandList::BeginTrackingImageState(FRHIImage* Image, FTextureSubresourceSet Subresources, EResourceStates StateBits)
    {
        StateTracker.BeginTrackingTextureState(Image, Subresources, StateBits);
    }

    void FVulkanCommandList::BeginTrackingBufferState(FRHIBuffer* Buffer, EResourceStates StateBits)
    {
        StateTracker.BeginTrackingBufferState(Buffer, StateBits);
    }

    void FVulkanCommandList::SetImageState(FRHIImage* Image, FTextureSubresourceSet Subresources, EResourceStates StateBits)
    {
        StateTracker.RequireTextureState(Image, Subresources, StateBits);

        if (CurrentCommandBuffer)
        {
            CurrentCommandBuffer->ReferencedResources.push_back(Image);
        }
    }

    void FVulkanCommandList::SetBufferState(FRHIBuffer* Buffer, EResourceStates StateBits)
    {
        StateTracker.RequireBufferState(Buffer, StateBits);
        
        if (CurrentCommandBuffer)
        {
            CurrentCommandBuffer->ReferencedResources.push_back(Buffer);
        }
    }

    EResourceStates FVulkanCommandList::GetImageSubresourceState(FRHIImage* Image, uint32 ArraySlice, uint32 MipLevel)
    {
        return StateTracker.GetTextureSubresourceState(Image, ArraySlice, MipLevel);
    }

    EResourceStates FVulkanCommandList::GetBufferState(FRHIBuffer* Buffer)
    {
        return StateTracker.GetBufferState(Buffer);
    }

    void FVulkanCommandList::CommitBarriers()
    {
        LUMINA_PROFILE_SCOPE();

        FVulkanPipelineBarrier Barrier;

        if (StateTracker.GetBufferBarriers().empty() && StateTracker.GetTextureBarriers().empty())
        {
            return;
        }

        EndRenderPass();

        CommitBarriersInternal();
        
    }

    void FVulkanCommandList::SetResourceStatesForBindingSet(FRHIBindingSet* BindingSet)
    {
        LUMINA_PROFILE_SCOPE();

        if (BindingSet == nullptr)
        {
            return;
        }

        if (BindingSet->GetDesc() == nullptr)
        {
            return; // Bindless.
        }
        

        FVulkanBindingSet* VkBindingSet = static_cast<FVulkanBindingSet*>(BindingSet);
        
        for (uint32 Binding : VkBindingSet->BindingsRequiringTransitions)
        {
            const FBindingSetItem& Item = VkBindingSet->Desc.Bindings[Binding];

            switch (Item.Type)
            {
                case ERHIBindingResourceType::Texture_SRV:
                    {
                        FVulkanImage* VulkanImage = static_cast<FVulkanImage*>(Item.ResourceHandle);
                        RequireTextureState(VulkanImage, Item.Subresources, EResourceStates::ShaderResource);
                    }
                    break;
                case ERHIBindingResourceType::Texture_UAV:
                    {
                        FVulkanImage* VulkanImage = static_cast<FVulkanImage*>(Item.ResourceHandle);
                        RequireTextureState(VulkanImage, Item.Subresources, EResourceStates::UnorderedAccess);
                    }
                    break;
                case ERHIBindingResourceType::Buffer_SRV:
                    {
                        FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Item.ResourceHandle);
                        RequireBufferState(Buffer, EResourceStates::ShaderResource);
                    }
                    break;
                case ERHIBindingResourceType::Buffer_UAV:
                    {
                        FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Item.ResourceHandle);
                        RequireBufferState(Buffer, EResourceStates::UnorderedAccess);
                    }
                    break;
                case ERHIBindingResourceType::Buffer_CBV:
                    {
                        FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(Item.ResourceHandle);
                        RequireBufferState(Buffer, EResourceStates::ConstantBuffer);
                    }
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

        if (GraphicsState.CurrentRenderPassInfo.bValidPass)
        {
            EndRenderPass();
        }
        
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
            Attachment.storeOp = (PassInfo.ColorStoreOps[i] == ERenderStoreOp::Store) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

            if (!PassInfo.ClearColorValues.empty())
            {
                Attachment.clearValue.color.float32[0] = PassInfo.ClearColorValues[i].R;
                Attachment.clearValue.color.float32[1] = PassInfo.ClearColorValues[i].G;
                Attachment.clearValue.color.float32[2] = PassInfo.ClearColorValues[i].B;
                Attachment.clearValue.color.float32[3] = PassInfo.ClearColorValues[i].A;
            }

            SetImageState(Image, AllSubresources, EResourceStates::RenderTarget);
            ColorAttachments[i] = Attachment;
        }
        
        
        const FRHIImageRef& ImageHandle = PassInfo.DepthAttachment;
        if (ImageHandle.IsValid())
        {
            TRefCountPtr<FVulkanImage> VulkanImage = PassInfo.DepthAttachment.As<FVulkanImage>();
            CurrentCommandBuffer->AddReferencedResource(VulkanImage);
            
            VkRenderingAttachmentInfo Attachment = {};
            Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            Attachment.imageView = VulkanImage->GetImageView();
            Attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL; 
            Attachment.loadOp = (PassInfo.DepthLoadOp == ERenderLoadOp::Clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            Attachment.storeOp = (PassInfo.DepthStoreOp == ERenderStoreOp::Store) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
            
            DepthAttachment = Attachment;
            DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            DepthAttachment.clearValue.depthStencil.depth = PassInfo.ClearDepth;
            DepthAttachment.clearValue.depthStencil.stencil = PassInfo.ClearStencil;
            if (PassInfo.DepthLoadOp == ERenderLoadOp::Clear || PassInfo.DepthStoreOp != ERenderStoreOp::DontCare)
            {
                SetImageState(VulkanImage, AllSubresources, EResourceStates::DepthWrite);
            }
            else
            {
                SetImageState(VulkanImage, AllSubresources, EResourceStates::DepthRead);
            }
        }

        CommitBarriers();
        
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
        GraphicsState.CurrentRenderPassInfo.bValidPass = true;
    }

    void FVulkanCommandList::EndRenderPass()
    {
        if (GraphicsState.CurrentRenderPassInfo.bValidPass)
        {
            LUMINA_PROFILE_SCOPE();
            TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "EndRenderPass")        

            vkCmdEndRendering(CurrentCommandBuffer->CommandBuffer);
            GraphicsState.CurrentRenderPassInfo = {};
        }
    }

    void FVulkanCommandList::ClearImageColor(FRHIImage* Image, const FColor& Color)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "ClearImageColor")        
        Assert(Image != nullptr)
        
        CurrentCommandBuffer->AddReferencedResource(Image);
        
        RequireTextureState(Image, FTextureSubresourceSet(0, 1, 0, 1), EResourceStates::CopyDest);
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
        
        vkCmdClearColorImage(CurrentCommandBuffer->CommandBuffer, Image->GetAPIResource<VkImage, EAPIResourceType::Image>(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &Value, 1, &Range);
    }

    void FVulkanCommandList::BindBindingSets(ERHIBindingPoint BindPoint, TVector<TPair<FRHIBindingSet*, uint32>> BindingSets)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "BindBindingSet")
        
        VkPipelineBindPoint BindingPoint = (BindPoint == ERHIBindingPoint::Graphics)
            ? VK_PIPELINE_BIND_POINT_GRAPHICS
            : VK_PIPELINE_BIND_POINT_COMPUTE;
    
        CurrentPipelineLayout = (BindPoint == ERHIBindingPoint::Graphics)
            ? GraphicsState.Pipeline->PipelineLayout
            : ComputeState.Pipeline->PipelineLayout;
    
        PushConstantVisibility = (BindPoint == ERHIBindingPoint::Graphics)
            ? GraphicsState.Pipeline->PushConstantVisibility
            : ComputeState.Pipeline->PushConstantVisibility;

        auto CurrentBindings = (BindPoint == ERHIBindingPoint::Graphics)
            ? GraphicsState.CurrentBindings
            : ComputeState.CurrentBindings;

        if (CurrentBindings == BindingSets)
        {
            return;
        }

        switch (BindPoint)
        {
        case ERHIBindingPoint::Graphics:
            {
                GraphicsState.CurrentBindings = BindingSets;
            }
            break;
        case ERHIBindingPoint::Compute:
            {
                ComputeState.CurrentBindings = BindingSets;
            }
            break;
        }
        
        std::ranges::sort(BindingSets, [](const auto& a, const auto& b)
        {
            return a.second < b.second;
        });
    
        uint32 CurrentBatchStart = UINT32_MAX;
        TVector<VkDescriptorSet> CurrentDescriptorBatch;
        TVector<uint32> DynamicOffsets;
    
        for (size_t i = 0; i < BindingSets.size(); ++i)
        {
            FRHIBindingSet* Set = BindingSets[i].first;
            uint32 SetIndex = BindingSets[i].second;
            FVulkanBindingSet* VulkanSet = static_cast<FVulkanBindingSet*>(Set);
    
            SetResourceStatesForBindingSet(VulkanSet);
            CommitBarriers();
    
            if (!VulkanSet)
            {
                continue;
            }
    
            if (CurrentBatchStart == UINT32_MAX)
            {
                CurrentBatchStart = SetIndex;
            }
    
            // Handle gaps — flush current batch if gap detected
            if (SetIndex != CurrentBatchStart + CurrentDescriptorBatch.size())
            {
                if (!CurrentDescriptorBatch.empty())
                {
                    vkCmdBindDescriptorSets(CurrentCommandBuffer->CommandBuffer,
                        BindingPoint,
                        CurrentPipelineLayout,
                        CurrentBatchStart,
                        static_cast<uint32>(CurrentDescriptorBatch.size()),
                        CurrentDescriptorBatch.data(),
                        static_cast<uint32>(DynamicOffsets.size()),
                        DynamicOffsets.data());
    
                    CurrentDescriptorBatch.clear();
                    DynamicOffsets.clear();
                    CurrentBatchStart = SetIndex;
                }
            }
    
            CurrentDescriptorBatch.push_back(VulkanSet->DescriptorSet);
    
            if (VulkanSet->GetDesc())
            {
                for (FRHIBuffer* DynamicBuffer : VulkanSet->DynamicBuffers)
                {
                    auto Found = DynamicBufferWrites.find(DynamicBuffer);
                    if (Found == DynamicBufferWrites.end())
                    {
                        LOG_ERROR("Binding [Dynamic Buffer] \"{0}\" before writing is invalid!", DynamicBuffer->GetDescription().DebugName);
                        DynamicOffsets.push_back(0);
                    }
                    else
                    {
                        uint32 Version = (uint32)Found->second.LatestVersion;
                        uint64 Offset = Version * DynamicBuffer->GetDescription().Size;
                        DynamicOffsets.push_back(static_cast<uint32>(Offset));
                    }
                }

                CurrentCommandBuffer->AddReferencedResource(VulkanSet);
            }
        }
        
        // Final flush
        if (!CurrentDescriptorBatch.empty())
        {
            vkCmdBindDescriptorSets(CurrentCommandBuffer->CommandBuffer,
                BindingPoint,
                CurrentPipelineLayout,
                CurrentBatchStart,
                static_cast<uint32>(CurrentDescriptorBatch.size()),
                CurrentDescriptorBatch.data(),
                static_cast<uint32>(DynamicOffsets.size()),
                DynamicOffsets.data());
        }
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
        Assert(Buffer != nullptr)
        
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
        Viewport.y        = MaxY;
        Viewport.width    = MaxX - MinX;
        Viewport.height   = -(MaxY - MinY);
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
        Scissor.extent.width = std::abs((int32)(MaxX - MinX));
        Scissor.extent.height = std::abs((int32)(MaxY - MinY));

        vkCmdSetScissor(CurrentCommandBuffer->CommandBuffer, 0, 1, &Scissor);
    }
    
    void FVulkanCommandList::Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "Draw")        

        GraphicsState.PrepareForDraw(CurrentCommandBuffer->CommandBuffer);
        vkCmdDraw(CurrentCommandBuffer->CommandBuffer, VertexCount, InstanceCount, FirstVertex, FirstInstance);
    }

    void FVulkanCommandList::DrawIndexed(FRHIBuffer* IndexBuffer, uint32 IndexCount, uint32 InstanceCount, uint32 FirstIndex, int32 VertexOffset, uint32 FirstInstance)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "DrawIndexed")
        
        FVulkanBuffer* Buffer = static_cast<FVulkanBuffer*>(IndexBuffer);
        CurrentCommandBuffer->AddReferencedResource(Buffer);
        
        if (Buffer->Buffer != GraphicsState.IndexBuffer)
        {
            vkCmdBindIndexBuffer(CurrentCommandBuffer->CommandBuffer, Buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
            GraphicsState.IndexBuffer = Buffer->Buffer;
        }
        
        GraphicsState.PrepareForDraw(CurrentCommandBuffer->CommandBuffer);
        vkCmdDrawIndexed(CurrentCommandBuffer->CommandBuffer, IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
    }

    void FVulkanCommandList::DrawIndirect(FRHIBuffer* Buffer, uint32 DrawCount, uint64 Offset)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "DrawIndirect")
        
        CurrentCommandBuffer->AddReferencedResource(Buffer);
        
        GraphicsState.PrepareForDraw(CurrentCommandBuffer->CommandBuffer);
        vkCmdDrawIndirect(CurrentCommandBuffer->CommandBuffer, Buffer->GetAPIResource<VkBuffer>(), Offset, DrawCount, sizeof(FDrawIndirectArguments));
    }

    void FVulkanCommandList::DrawIndexedIndirect(FRHIBuffer* DrawBuffer, FRHIBuffer* IndexBuffer, uint32 DrawCount, uint64 Offset)
    {
        LUMINA_PROFILE_SCOPE();
        TracyVkZone(CurrentCommandBuffer->TracyContext, CurrentCommandBuffer->CommandBuffer, "DrawIndexedIndirect")
        Assert(GraphicsState.CurrentRenderPassInfo.bValidPass)
        
        CurrentCommandBuffer->AddReferencedResource(DrawBuffer);
        CurrentCommandBuffer->AddReferencedResource(IndexBuffer);
        
        VkBuffer IndexBufferVk = IndexBuffer->GetAPIResource<VkBuffer>();
        if (IndexBufferVk != GraphicsState.IndexBuffer)
        {
            vkCmdBindIndexBuffer(CurrentCommandBuffer->CommandBuffer, IndexBufferVk, 0, VK_INDEX_TYPE_UINT32);
            GraphicsState.IndexBuffer = IndexBufferVk;
        }
        
        GraphicsState.PrepareForDraw(CurrentCommandBuffer->CommandBuffer);
        vkCmdDrawIndexedIndirect(CurrentCommandBuffer->CommandBuffer, DrawBuffer->GetAPIResource<VkBuffer>(), Offset, DrawCount, sizeof(FDrawIndexedIndirectArguments));
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

        vkCmdDispatch(CurrentCommandBuffer->CommandBuffer, GroupCountX, GroupCountY, GroupCountZ);
    }

    void FVulkanCommandList::CommitBarriersInternal()
    {
        LUMINA_PROFILE_SCOPE();

        TVector<VkImageMemoryBarrier> imageBarriers;
        TVector<VkBufferMemoryBarrier> bufferBarriers;
        VkPipelineStageFlags beforeStageFlags = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
        VkPipelineStageFlags afterStageFlags = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;

        for (const FTextureBarrier& barrier : StateTracker.GetTextureBarriers())
        {
            FResourceStateMapping before = Vk::ConvertResourceState(barrier.StateBefore);
            FResourceStateMapping after = Vk::ConvertResourceState(barrier.StateAfter);

            if ((before.StageFlags != beforeStageFlags || after.StageFlags != afterStageFlags) && !imageBarriers.empty())
            {
                vkCmdPipelineBarrier(
                    CurrentCommandBuffer->CommandBuffer,
                    beforeStageFlags, afterStageFlags,
                    0, // dependencyFlags
                    0, nullptr, // memory barriers
                    0, nullptr, // buffer barriers
                    static_cast<uint32>(imageBarriers.size()), imageBarriers.data()
                );

                imageBarriers.clear();
            }

            beforeStageFlags = before.StageFlags;
            afterStageFlags = after.StageFlags;

            Assert(after.ImageLayout != VK_IMAGE_LAYOUT_UNDEFINED);

            FVulkanImage* texture = static_cast<FVulkanImage*>(barrier.Texture);

            const FFormatInfo& formatInfo = GetFormatInfo(texture->GetDescription().Format);

            VkImageAspectFlags aspectMask = 0;
            if (formatInfo.bHasDepth)   aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
            if (formatInfo.bHasStencil) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            if (!aspectMask)            aspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;

            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.baseArrayLayer = barrier.bEntireTexture ? 0 : barrier.ArraySlice;
            subresourceRange.layerCount     = barrier.bEntireTexture ? texture->GetDescription().ArraySize : 1;
            subresourceRange.baseMipLevel   = barrier.bEntireTexture ? 0 : barrier.MipLevel;
            subresourceRange.levelCount     = barrier.bEntireTexture ? texture->GetDescription().NumMips : 1;
            subresourceRange.aspectMask     = aspectMask;


            VkImageMemoryBarrier imgBarrier = {};
            imgBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imgBarrier.srcAccessMask       = before.AccessMask;
            imgBarrier.dstAccessMask       = after.AccessMask;
            imgBarrier.oldLayout           = before.ImageLayout;
            imgBarrier.newLayout           = after.ImageLayout;
            imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imgBarrier.image               = texture->GetAPIResource<VkImage, EAPIResourceType::Image>();
            imgBarrier.subresourceRange    = subresourceRange;

            imageBarriers.push_back(imgBarrier);
        }
        
        if (!imageBarriers.empty())
        {
            vkCmdPipelineBarrier(
                CurrentCommandBuffer->CommandBuffer,
                beforeStageFlags, afterStageFlags,
                0, // dependencyFlags
                0, nullptr, // memory barriers
                0, nullptr, // buffer barriers
                static_cast<uint32_t>(imageBarriers.size()), imageBarriers.data()
            );
        }

        beforeStageFlags = 0;
        afterStageFlags = 0;
        imageBarriers.clear();
        
        for (const FBufferBarrier& barrier : StateTracker.GetBufferBarriers())
        {
            FResourceStateMapping before = Vk::ConvertResourceState(barrier.StateBefore);
            FResourceStateMapping after = Vk::ConvertResourceState(barrier.StateAfter);
        
            if ((before.StageFlags != beforeStageFlags || after.StageFlags != afterStageFlags) && !bufferBarriers.empty())
            {
                vkCmdPipelineBarrier(
                    CurrentCommandBuffer->CommandBuffer,
                    beforeStageFlags, afterStageFlags,
                    0, // dependencyFlags
                    0, nullptr, // memory barriers
                    static_cast<uint32_t>(bufferBarriers.size()), bufferBarriers.data(),
                    0, nullptr // image barriers
                );
        
                bufferBarriers.clear();
            }
        
            beforeStageFlags = before.StageFlags;
            afterStageFlags = after.StageFlags;
        
            FVulkanBuffer* buffer = static_cast<FVulkanBuffer*>(barrier.Buffer);
        
            VkBufferMemoryBarrier bufBarrier = {};
            bufBarrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            bufBarrier.srcAccessMask       = before.AccessMask;
            bufBarrier.dstAccessMask       = after.AccessMask;
            bufBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufBarrier.buffer              = buffer->Buffer;
            bufBarrier.offset              = 0;
            bufBarrier.size                = buffer->GetDescription().Size;
        
            bufferBarriers.push_back(bufBarrier);
        }
        
        if (!bufferBarriers.empty())
        {
            vkCmdPipelineBarrier(
                CurrentCommandBuffer->CommandBuffer,
                beforeStageFlags, afterStageFlags,
                0, // dependencyFlags
                0, nullptr, // memory barriers
                static_cast<uint32>(bufferBarriers.size()), bufferBarriers.data(),
                0, nullptr // image barriers
            );
        }
        
        bufferBarriers.clear();
        StateTracker.ClearBarriers();
    }

    void FVulkanCommandList::TrackResourcesAndBarriers()
    {
    }

    void FVulkanCommandList::RequireTextureState(FRHIImage* Texture, FTextureSubresourceSet Subresources, EResourceStates StateBits)
    {
        StateTracker.RequireTextureState(Texture, Subresources, StateBits);
    }

    void FVulkanCommandList::RequireBufferState(FRHIBuffer* Buffer, EResourceStates StateBits)
    {
        StateTracker.RequireBufferState(Buffer, StateBits);
    }

    void* FVulkanCommandList::GetAPIResourceImpl(EAPIResourceType Type)
    {
        return CurrentCommandBuffer->CommandBuffer;
    }
}
