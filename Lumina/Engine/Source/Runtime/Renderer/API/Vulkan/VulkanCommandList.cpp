#include "VulkanCommandList.h"

#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "VulkanResources.h"
#include "Renderer/RenderContext.h"

namespace Lumina
{
    void FVulkanCommandList::Open()
    {
        CurrentCommandBuffer = RenderContext->GetQueue(Info.CommandQueue)->GetOrCreateCommandBuffer();
        
        VkCommandBufferBeginInfo BeginInfo = {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        BeginInfo.pInheritanceInfo = nullptr;
        
        VK_CHECK(vkBeginCommandBuffer(CurrentCommandBuffer->CommandBuffer, &BeginInfo));

        bRecording = true;
    }

    void FVulkanCommandList::Close()
    {
        bRecording = false;
        
        VK_CHECK(vkEndCommandBuffer(CurrentCommandBuffer->CommandBuffer));
    }

    void FVulkanCommandList::Executed(FQueue* Queue)
    {
        
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
        VmaAllocationCreateFlags VmaFlags = 0;
        
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = Size;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        BufferCreateInfo.flags = 0;


        TBitFlags<EBufferUsageFlags> BufferUsage;
        BufferUsage.SetFlag(EBufferUsageFlags::StagingBuffer);
        
        FRHIBufferDesc Description;
        Description.Size = Size;
        Description.Stride = Size;
        Description.Usage = BufferUsage;

        /*FRHIBufferRef StagingBuffer = CreateBuffer(Description);
        VmaAllocation Allocation = VulkanDevice->GetAllocator()->GetAllocation(StagingBuffer.As<FVulkanBuffer>()->GetBuffer());
        
        void* Memory = VulkanDevice->GetAllocator()->MapMemory(Allocation);
        FMemory::MemCopy((char*)Memory + Offset, Data, Size);
        VulkanDevice->GetAllocator()->UnmapMemory(Allocation);

        CopyBuffer(CommandList, StagingBuffer, Buffer);*/
    }

    void FVulkanCommandList::BeginRenderPass(const FRenderPassBeginInfo& PassInfo)
    {
        Assert(bRecording);
        
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
    }

    void FVulkanCommandList::ClearColor(const FColor& Color)
    {
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
