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
        
        void Destroy() override;

        VkDescriptorSet GetSet() { return DescriptorSet; }
        VkDescriptorSetLayout GetLayout() { return Layout; }
        
        void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset) override;
        void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler) override;
        void Write(uint16 Binding, uint16 ArrayElement, TFastVector<TRefPtr<FImage>> Images, TRefPtr<FImageSampler> Sampler) override;

    private:

        VkDescriptorSet DescriptorSet;
        VkDescriptorSetLayout Layout;
        
    };
}