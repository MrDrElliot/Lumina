#pragma once
#include <vulkan/vulkan_core.h>

#include "Source/Runtime/Renderer/DescriptorSet.h"

namespace Lumina
{
    class FVulkanDescriptorSet : public FDescriptorSet
    {
    public:


        FVulkanDescriptorSet(const FDescriptorSetSpecification& InSpec);
        ~FVulkanDescriptorSet() override;

        void* GetPlatformDescriptorSet() const override { return DescriptorSet; }
        void* GetPlatformDescriptorSetLayout() const override { return Layout; }
        
        void Write(uint16 Binding, uint16 ArrayElement, FRHIBuffer Buffer, uint64 Size, uint64 Offset) override;
        void Write(uint16 Binding, uint16 ArrayElement, FRHIImage Image) override;
        void Write(uint16 Binding, uint16 ArrayElement, TVector<FRHIImage> Images) override;

        void SetFriendlyName(const FString& InName) override;

    private:

        VkDescriptorSet DescriptorSet;
        VkDescriptorSetLayout Layout;
        
    };
}
