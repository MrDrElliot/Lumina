#include "VulkanShader.h"

#include "VulkanRenderContext.h"
#include "Source/Runtime/Log/Log.h"
#include "SPIRV-Reflect/spirv_reflect.h"

namespace Lumina
{


		constexpr VkShaderStageFlagBits convert(const EShaderStage& stage)
		{
			switch (stage)
			{
				case EShaderStage::VERTEX:		return VK_SHADER_STAGE_VERTEX_BIT;
				case EShaderStage::FRAGMENT:	return VK_SHADER_STAGE_FRAGMENT_BIT;
				case EShaderStage::COMPUTE:		return VK_SHADER_STAGE_COMPUTE_BIT;
			}
		}

	constexpr VkDescriptorType convert(SpvReflectDescriptorType type)
	{
		switch (type)
		{
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:							return VK_DESCRIPTOR_TYPE_SAMPLER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:						return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:						return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:				return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:				return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:					return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			default:															std::unreachable();
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		}
	}

	constexpr std::string DescriptorToString(VkDescriptorType type)
	{
		switch (type)
		{
			case VK_DESCRIPTOR_TYPE_SAMPLER:						return "sampler";
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:			return "combined image sampler";
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:					return "sampled image";
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:					return "storage image";
			case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:			return "uniform texel buffer";
			case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:			return "storage texel buffer";
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:					return "uniform buffer";
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:					return "storage buffer";
			default:												std::unreachable();
		}
	}
	
    FVulkanShader::FVulkanShader(std::map<EShaderStage, std::vector<glm::uint32>> Binaries, std::filesystem::path Path)
    {
        auto Device = FVulkanRenderContext::GetDevice();

        LE_LOG_INFO("===============================");
        LE_LOG_INFO("Reflecting Shader - {0}", Path.filename().string());
        LE_LOG_INFO("===============================");

		std::map<glm::uint32, std::vector<VkDescriptorSetLayoutBinding>> Bindings;
		VkPushConstantRange PushConstantRange = {};

        for (auto& StageData : Binaries)
        {
			VkShaderModule ShaderModule;

			VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
			ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			ShaderModuleCreateInfo.pCode = StageData.second.data();
			ShaderModuleCreateInfo.codeSize = StageData.second.size() * 4;
			
			vkCreateShaderModule(Device, &ShaderModuleCreateInfo, nullptr, &ShaderModule);

			VkPipelineShaderStageCreateInfo ShaderStageCreateInfo = {};
			ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ShaderStageCreateInfo.stage = convert(StageData.first);
			ShaderStageCreateInfo.pName = "main";
			ShaderStageCreateInfo.module = ShaderModule;

			StageCreateInfos.push_back(ShaderStageCreateInfo);
			SpvReflectShaderModule reflect_module;
			if (spvReflectCreateShaderModule(StageData.second.size() * 4, StageData.second.data(), &reflect_module) != SPV_REFLECT_RESULT_SUCCESS)
			{
				LE_LOG_ERROR("Failed to reflect shader {0}", Path.filename().string());
				bDirty = true;
				return;
			}

			glm::uint32 SetCount = 0;
			spvReflectEnumerateDescriptorSets(&reflect_module, &SetCount, nullptr);

			std::vector<SpvReflectDescriptorSet*> ReflectDescriptorSets(SetCount);
			spvReflectEnumerateDescriptorSets(&reflect_module, &SetCount, ReflectDescriptorSets.data());

			for (auto& ReflectSet : ReflectDescriptorSets)
			{
				if (Bindings.find(ReflectSet->set) != Bindings.end())
				{
					Bindings.emplace(ReflectSet->set, std::vector<VkDescriptorSetLayoutBinding>());
				}

				for (int i = 0; i < ReflectSet->binding_count; i++)
				{
					SpvReflectDescriptorBinding* reflect_binding = ReflectSet->bindings[i];

					VkDescriptorSetLayoutBinding layout_binding = {};
					layout_binding.binding = reflect_binding->binding;
					layout_binding.descriptorType = convert(reflect_binding->descriptor_type);
					layout_binding.descriptorCount = reflect_binding->count;
					layout_binding.stageFlags = VK_SHADER_STAGE_ALL;

					for (auto& set_binding : Bindings[ReflectSet->set])
					{
						if (set_binding.binding == layout_binding.binding)
						{
							continue;
						}
					}
					
					Bindings[ReflectSet->set].push_back(layout_binding);
				}
			}

			glm::uint32 PushConstantRangeCount = 0;
			spvReflectEnumeratePushConstantBlocks(&reflect_module, &PushConstantRangeCount, nullptr);

			std::vector<SpvReflectBlockVariable*> ReflectPushConstantRanges(PushConstantRangeCount);
			spvReflectEnumeratePushConstantBlocks(&reflect_module, &PushConstantRangeCount, ReflectPushConstantRanges.data());

			for (auto& reflect_range : ReflectPushConstantRanges)
			{
				PushConstantRange.size = reflect_range->size; // TODO: some dodgy stuff here
				PushConstantRange.offset = 0;
				PushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
			}

			spvReflectDestroyShaderModule(&reflect_module);
		}

		if(PushConstantRange.size)
		{
			Ranges.push_back(PushConstantRange);
		}

			for (auto& set : Bindings)
			{
				LE_LOG_TRACE("\tSet #{0}: ", set.first);
				for (auto& binding : set.second)
				{
					LE_LOG_TRACE("\t\t Binding #{0}: {1}[{2}]", binding.binding, DescriptorToString(binding.descriptorType), binding.descriptorCount);
				}
			}
			printf("\n");

		if (Bindings.size())
		{
			glm::uint32 highest_set_index = Bindings.rbegin()->first;
			for (int i = 0; i <= highest_set_index; i++)
			{
				if (Bindings.find(i) == Bindings.end())
				{
					SetLayouts.push_back(VK_NULL_HANDLE);
				}
				else
				{
					std::vector<VkDescriptorBindingFlags> binding_flags;
					for (auto& binding : Bindings[i])
					{
						if (binding.descriptorCount != 1) binding_flags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
						else binding_flags.push_back(0);
					}

					VkDescriptorSetLayoutBindingFlagsCreateInfo layout_bindings_flags = {};
					layout_bindings_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
					layout_bindings_flags.bindingCount = binding_flags.size();
					layout_bindings_flags.pBindingFlags = binding_flags.data();

					VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
					LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					LayoutCreateInfo.pNext = &layout_bindings_flags;
					LayoutCreateInfo.bindingCount = Bindings[i].size();
					LayoutCreateInfo.pBindings = Bindings[i].data();

					VkDescriptorSetLayout vk_descriptor_set_layout = VK_NULL_HANDLE;
					vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &vk_descriptor_set_layout);
					SetLayouts.push_back(vk_descriptor_set_layout);
				}
			}
		}

    }
    

    FVulkanShader::~FVulkanShader()
    {
    }

    void FVulkanShader::RestoreShaderModule(std::filesystem::path path)
    {
    }

    void FVulkanShader::Destroy()
    {
    }
}
