#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanRenderAPI.h"
#include "VulkanRenderContext.h"
#include "Source/Runtime/Renderer/Buffer.h"

namespace Lumina
{

    constexpr VkDescriptorType UsageToDescriptorType(const EDeviceBufferUsage& usage)
    {
        switch (usage)
        {
        case EDeviceBufferUsage::UNIFORM_BUFFER:		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case EDeviceBufferUsage::STORAGE_BUFFER:		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        default:									    std::unreachable();
        }
    }

    constexpr VkDescriptorType convert(const EDescriptorBindingType& type)
    {
        switch (type)
        {
        case EDescriptorBindingType::SAMPLED_IMAGE:		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case EDescriptorBindingType::STORAGE_IMAGE:		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case EDescriptorBindingType::UNIFORM_BUFFER:	return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case EDescriptorBindingType::STORAGE_BUFFER:	return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        default:										std::unreachable();
        }
    }


    constexpr VkDescriptorBindingFlags extractFlags(const glm::uint64& Mask)
    {
        VkDescriptorBindingFlags Flags = 0;

        if (Mask & (glm::uint32)EDescriptorFlags::PARTIALLY_BOUND)
        {
            Flags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
        }

        return Flags;
    }
    
    FVulkanDescriptorSet::FVulkanDescriptorSet(const FDescriptorSetSpecification& InSpec)
    : DescriptorSet(VK_NULL_HANDLE), Layout(VK_NULL_HANDLE)
    {
        auto Device = FVulkanRenderContext::GetDevice();

        std::vector<VkDescriptorSetLayoutBinding> SetBindings;
        std::vector<VkDescriptorBindingFlags> BindingFlags;

        for (auto& binding : InSpec.Bindings)
         {
            VkDescriptorSetLayoutBinding VkBinding = {};
            VkBinding.stageFlags = VK_SHADER_STAGE_ALL;
            VkBinding.pImmutableSamplers = nullptr;
            VkBinding.descriptorCount = binding.ArrayCount;
            VkBinding.descriptorType = convert(binding.Type);
            VkBinding.binding = binding.Binding;

            SetBindings.push_back(VkBinding);
            BindingFlags.push_back(extractFlags(binding.Flags));
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo VkBindingFlags = {};
        VkBindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        VkBindingFlags.bindingCount = InSpec.Bindings.size();
        VkBindingFlags.pBindingFlags = BindingFlags.data();

        VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
        LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        LayoutCreateInfo.pNext = &VkBindingFlags;
        LayoutCreateInfo.bindingCount = SetBindings.size();
        LayoutCreateInfo.pBindings = SetBindings.data();
        LayoutCreateInfo.flags = 0;

        vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &Layout);

        std::vector<VkDescriptorSet> set = FVulkanRenderAPI::AllocateDescriptorSets(Layout, 1);
        DescriptorSet = set[0];
    }

    FVulkanDescriptorSet::~FVulkanDescriptorSet()
    {
        auto Device = FVulkanRenderContext::GetDevice();
        vkDestroyDescriptorSetLayout(Device, Layout, nullptr);
        FVulkanRenderAPI::FreeDescriptorSets({ DescriptorSet });
    }

    void FVulkanDescriptorSet::Destroy()
    {
        auto Device = FVulkanRenderContext::GetDevice();
        vkDestroyDescriptorSetLayout(Device, Layout, nullptr);
        FVulkanRenderAPI::FreeDescriptorSets({ DescriptorSet });
    }

    void FVulkanDescriptorSet::Write(glm::uint16 Binding, glm::uint16 ArrayElement, std::shared_ptr<FBuffer> Buffer, glm::uint64 Size, glm::uint64 Offset)
    {
        auto Device = FVulkanRenderContext::GetDevice();
        
        std::shared_ptr<FVulkanBuffer> vk_buffer = std::dynamic_pointer_cast<FVulkanBuffer>(Buffer);

        VkDescriptorBufferInfo DescriptorBufferInfo = {};
        DescriptorBufferInfo.buffer = vk_buffer->GetBuffer();
        DescriptorBufferInfo.range = Size;
        DescriptorBufferInfo.offset = Offset;

        VkWriteDescriptorSet WriteDescriptorSet = {};
        WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptorSet.dstSet = DescriptorSet;
        WriteDescriptorSet.dstBinding = Binding;
        WriteDescriptorSet.descriptorType = UsageToDescriptorType(vk_buffer->GetSpecification().BufferUsage);
        WriteDescriptorSet.dstArrayElement = ArrayElement;
        WriteDescriptorSet.descriptorCount = 1;
        WriteDescriptorSet.pBufferInfo = &DescriptorBufferInfo;

        vkUpdateDescriptorSets(Device, 1, &WriteDescriptorSet, 0, nullptr);
    }

    void FVulkanDescriptorSet::Write(glm::uint16 Binding, glm::uint16 ArrayElement, std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler)
    {
        auto Device = FVulkanRenderContext::GetDevice();
        std::shared_ptr<FVulkanImage> vk_image = std::dynamic_pointer_cast<FVulkanImage>(Image);
        std::shared_ptr<FVulkanImageSampler> vk_sampler = std::dynamic_pointer_cast<FVulkanImageSampler>(Sampler);

        VkDescriptorImageInfo DescriptorImageInfo = {};
        DescriptorImageInfo.imageView = vk_image->GetImageView();;
        DescriptorImageInfo.sampler = vk_sampler->GetSampler();
        DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet WriteDescriptorSet = {};
        WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptorSet.dstSet = DescriptorSet;
        WriteDescriptorSet.dstBinding = Binding;
        WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        WriteDescriptorSet.dstArrayElement = ArrayElement;
        WriteDescriptorSet.descriptorCount = 1;
        WriteDescriptorSet.pImageInfo = &DescriptorImageInfo;

        vkUpdateDescriptorSets(Device, 1, &WriteDescriptorSet, 0, nullptr);
    }
}
