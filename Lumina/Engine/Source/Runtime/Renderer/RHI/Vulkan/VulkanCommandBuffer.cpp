
#include "VulkanCommandBuffer.h"

#include "VulkanMacros.h"
#include "Renderer/RHIIncl.h"

#include "VulkanSwapchain.h"
#include "VulkanRenderContext.h"

namespace Lumina
{
    FVulkanCommandBuffer::FVulkanCommandBuffer(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType)
    : CommandBuffer(VK_NULL_HANDLE), FCommandBuffer(InLevel, InBufferType, InCmdType)
    {
        //FQueueFamilyIndex Index = FRenderContext::GetQueueFamilyIndex();

        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

        
        VkDevice Device = RenderContext->GetDevice();
        
        /*
        VkCommandPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        PoolInfo.queueFamilyIndex = Index.Graphics;
        PoolInfo.flags = InBufferType == ECommandBufferType::GENERAL ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
        vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool);
        */
        
        VkCommandBufferAllocateInfo BufferInfo =  {};
        BufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        BufferInfo.commandPool = RenderContext->GetCommandPool();
        BufferInfo.commandBufferCount = 1;
        BufferInfo.level = Level == ECommandBufferLevel::PRIMARY ?  VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        VK_CHECK(vkAllocateCommandBuffers(Device, &BufferInfo, &CommandBuffer));
        
    }

    FVulkanCommandBuffer::~FVulkanCommandBuffer()
    {
    }

    void FVulkanCommandBuffer::SetFriendlyName(const FString& InName)
    {
        FCommandBuffer::SetFriendlyName(InName);
        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

        
        VkDevice Device = RenderContext->GetDevice();
        
        VkDebugUtilsObjectNameInfoEXT NameInfo = {};
        NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        NameInfo.pObjectName = GetFriendlyName().c_str();
        NameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
        NameInfo.objectHandle = reinterpret_cast<uint64_t>(CommandBuffer);


        RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
        
    }

    void FVulkanCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        Info.flags = BufferType == ECommandBufferType::TRANSIENT ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;

        VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &Info));
    }

    void FVulkanCommandBuffer::End()
    {
        VK_CHECK(vkEndCommandBuffer(CommandBuffer));
    }

    void FVulkanCommandBuffer::Reset()
    {
        VK_CHECK(vkResetCommandBuffer(CommandBuffer, 0));
    }

    void FVulkanCommandBuffer::Execute(bool bWait)
    {
        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;

        FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
        VkDevice Device = RenderContext->GetDevice();
        VkQueue Queue = RenderContext->GetGeneralQueue();

        VkFence Fence = VK_NULL_HANDLE;
        if(bWait)
        {
            VkFenceCreateInfo FenceInfo = {};
            FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

            VK_CHECK(vkCreateFence(Device, &FenceInfo, nullptr, &Fence));
        }
        
        VK_CHECK(vkQueueSubmit(Queue, 1, &SubmitInfo, Fence));

        if(bWait)
        {
            VK_CHECK(vkWaitForFences(Device, 1, &Fence, VK_TRUE, UINT64_MAX));
            vkDestroyFence(Device, Fence, nullptr);
        }
    }
}
