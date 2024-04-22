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
        
        void Write(glm::uint16 Binding, glm::uint16 ArrayElement, std::shared_ptr<FBuffer> Buffer, glm::uint64 Size, glm::uint64 Offset) override;
        void Write(glm::uint16 Binding, glm::uint16 ArrayElement, std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler) override;


    private:

        VkDescriptorSet DescriptorSet;
        VkDescriptorSetLayout Layout;
        
    };
}
