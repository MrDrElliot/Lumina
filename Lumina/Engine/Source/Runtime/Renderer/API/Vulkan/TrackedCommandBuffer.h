﻿#pragma once

#include "VulkanDevice.h"
#include "Memory/RefCounted.h"
#include "Renderer/RenderResource.h"
#include <tracy/TracyVulkan.hpp>

namespace Lumina
{
    class FTrackedCommandBuffer : public FRefCounted, public IDeviceChild
    {
    public:

        FTrackedCommandBuffer(FVulkanDevice* InDevice, VkCommandBuffer InBuffer, VkCommandPool InPool, bool bCreateTraceContext, VkQueue InQueue)
            : IDeviceChild(InDevice)
            , CommandBuffer(InBuffer)
            , CommandPool(InPool)
            , Queue(InQueue)
        {
            ReferencedResources.reserve(24);
            if (bCreateTraceContext)
            {
                TracyContext = TracyVkContext(InDevice->GetPhysicalDevice(),
                    InDevice->GetDevice(),
                    InQueue,
                    CommandBuffer)
            }
        }

        ~FTrackedCommandBuffer() override
        {
            if (TracyContext)
            {
                TracyVkDestroy(TracyContext)
            }
            vkDestroyCommandPool(Device->GetDevice(), CommandPool, nullptr);
        }

        void AddReferencedResource(const TRefCountPtr<IRHIResource>& InResource)
        {
            ReferencedResources.push_back(InResource);
        }

        void AddStagingResource(const TRefCountPtr<FRHIBuffer>& InResource)
        {
            ReferencedStagingResources.push_back(InResource);
        }
        
        void ClearReferencedResources()
        {
            ReferencedResources.clear();
            ReferencedStagingResources.clear();
        }

        VkCommandBuffer             CommandBuffer;
        VkCommandPool               CommandPool;
        VkQueue                     Queue;

        uint64                      SubmissionID = 0;
        uint64                      RecordingID = 0;

        TracyVkCtx                  TracyContext = nullptr;

        
        /** Here we keep alive any resources that this current command buffer needs/uses */
        TFixedVector<TRefCountPtr<IRHIResource>, 20> ReferencedResources;
        TFixedVector<TRefCountPtr<FRHIBuffer>, 20> ReferencedStagingResources;

    };
}
