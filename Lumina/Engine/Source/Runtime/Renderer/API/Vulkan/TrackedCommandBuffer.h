#pragma once

#include "VulkanDevice.h"
#include "Memory/RefCounted.h"
#include "Renderer/RenderResource.h"
#include <tracy/TracyVulkan.hpp>

namespace Lumina
{
    class FTrackedCommandBuffer : public FRefCounted, public IDeviceChild
    {
    public:

        FTrackedCommandBuffer(FVulkanDevice* InDevice, VkCommandBuffer InBuffer, VkCommandPool InPool, bool bCreateTraceContext, VkQueue InQueue);

        ~FTrackedCommandBuffer() override;

        void AddReferencedResource(const TRefCountPtr<IRHIResource>& InResource);

        void AddStagingResource(const TRefCountPtr<FRHIBuffer>& InResource);
        
        void ClearReferencedResources();

        VkCommandBuffer             CommandBuffer;
        VkCommandPool               CommandPool;
        VkQueue                     Queue;

        uint64                      SubmissionID = 0;
        uint64                      RecordingID = 0;

        TracyVkCtx                  TracyContext = nullptr;

        
        /** Here we keep alive any resources that this current command buffer needs/uses */
        TFixedVector<TRefCountPtr<IRHIResource>, 20>    ReferencedResources;
        TFixedVector<TRefCountPtr<FRHIBuffer>, 20>      ReferencedStagingResources;

    };
}
