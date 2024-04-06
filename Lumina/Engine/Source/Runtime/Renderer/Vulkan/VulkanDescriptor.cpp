#include "VulkanDescriptor.h"

#include "VulkanHelpers.h"

namespace Lumina
{
    void FDescriptorLayoutBuilder::AddBinding(uint32_t InBinding, VkDescriptorType InType)
    {
        VkDescriptorSetLayoutBinding Binding = {};
        Binding.binding = InBinding;
        Binding.descriptorCount = 1;
        Binding.descriptorType = InType;

        Bindings.push_back(Binding);
    }

    void FDescriptorLayoutBuilder::Clear()
    {
        Bindings.clear();
    }

    VkDescriptorSetLayout FDescriptorLayoutBuilder::Build(VkDevice InDevice, VkShaderStageFlags InShaderStages)
    {
       for(auto& Binding : Bindings)
       {
           Binding.stageFlags |= InShaderStages;
       }

        VkDescriptorSetLayoutCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        Info.pNext = nullptr;
        Info.pBindings = Bindings.data();
        Info.bindingCount = static_cast<uint32_t>(Bindings.size());
        Info.flags = 0;

        VkDescriptorSetLayout Set;

        VK_CHECK(vkCreateDescriptorSetLayout(InDevice, &Info, nullptr, &Set));

        return Set;
    }

    void FDescriptorAllocator::InitPool(VkDevice InDevice, uint32_t MaxSets, std::span<FPoolSizeRatio> PoolRatios)
    {
        std::vector<VkDescriptorPoolSize> PoolSizes;

        for(auto Ratio : PoolRatios)
        {
            PoolSizes.push_back(VkDescriptorPoolSize { .type = Ratio.Type, .descriptorCount = static_cast<uint32_t>(Ratio.Ratio * MaxSets) });
        }

        VkDescriptorPoolCreateInfo PoolInfo = {};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.pNext = nullptr;
        PoolInfo.flags = 0;
        PoolInfo.maxSets = MaxSets;
        PoolInfo.poolSizeCount = static_cast<uint32_t>(PoolSizes.size());
        PoolInfo.pPoolSizes = PoolSizes.data();

        vkCreateDescriptorPool(InDevice, &PoolInfo, nullptr, &Pool);
    }
    

    void FDescriptorAllocator::ClearDescriptors(VkDevice InDevice)
    {
        vkResetDescriptorPool(InDevice, Pool, 0);
    }

    void FDescriptorAllocator::DestroyPool(VkDevice InDevice)
    {
        vkDestroyDescriptorPool(InDevice, Pool, nullptr);
    }

    VkDescriptorSet FDescriptorAllocator::Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout)
    {
        VkDescriptorSetAllocateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        Info.pNext = nullptr;
        Info.descriptorPool = Pool;
        Info.descriptorSetCount = 1;
        Info.pSetLayouts = &InLayout;

        VkDescriptorSet Set;
        VK_CHECK(vkAllocateDescriptorSets(InDevice, &Info, &Set));

        return Set;
    }
}
