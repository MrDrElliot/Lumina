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
	
	constexpr VkDescriptorType convert(SpvReflectDescriptorType Type)
	{
		switch (Type)
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
	
    FVulkanShader::FVulkanShader(std::vector<FShaderData> InData, const std::string& Tag)
	{
		auto Device = FVulkanRenderContext::GetDevice();

		std::map<glm::uint32, std::vector<VkDescriptorSetLayoutBinding>> Bindings;
		VkPushConstantRange PushConstantRange = {};


		LE_LOG_INFO("===============================");
		LE_LOG_INFO("Reflecting Shader - {0}", Tag);
		LE_LOG_INFO("===============================");


		for (auto& StageData : InData)
		{
			VkShaderModule ShaderModule;

			VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
			ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			ShaderModuleCreateInfo.pCode = StageData.Binaries.data();
			ShaderModuleCreateInfo.codeSize = StageData.Binaries.size() * 4;
			
			vkCreateShaderModule(Device, &ShaderModuleCreateInfo, nullptr, &ShaderModule);

			VkPipelineShaderStageCreateInfo ShaderStageCreateInfo = {};
			ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ShaderStageCreateInfo.stage = convert(StageData.Stage);
			ShaderStageCreateInfo.pName = "main";
			ShaderStageCreateInfo.module = ShaderModule;

			StageCreateInfos.push_back(ShaderStageCreateInfo);

			SpvReflectShaderModule ReflectModule;
			if (spvReflectCreateShaderModule(StageData.Binaries.size() * 4, StageData.Binaries.data(), &ReflectModule) != SPV_REFLECT_RESULT_SUCCESS)
			{
				LE_LOG_ERROR("Failed to reflect shader {}", Path.filename().string());
				bDirty = true;
				return;
			}

			glm::uint32 set_count = 0;
			spvReflectEnumerateDescriptorSets(&ReflectModule, &set_count, nullptr);

			std::vector<SpvReflectDescriptorSet*> ReflectDescriptorSets(set_count);
			spvReflectEnumerateDescriptorSets(&ReflectModule, &set_count, ReflectDescriptorSets.data());

			for (auto& ReflectSet : ReflectDescriptorSets)
			{
				if (!Bindings.contains(ReflectSet->set))
				{
					Bindings.emplace(ReflectSet->set, std::vector<VkDescriptorSetLayoutBinding>());
				}

				for (int i = 0; i < ReflectSet->binding_count; i++)
				{
					SpvReflectDescriptorBinding* ReflectBinding = ReflectSet->bindings[i];

					VkDescriptorSetLayoutBinding layout_binding = {};
					layout_binding.binding = ReflectBinding->binding;
					layout_binding.descriptorType = convert(ReflectBinding->descriptor_type);
					layout_binding.descriptorCount = ReflectBinding->count;
					layout_binding.stageFlags = StageData.Stage == EShaderStage::COMPUTE ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL;

					bool bSkipBinding = false;

					for (auto& set_binding : Bindings[ReflectSet->set])
					{
						if (set_binding.binding == layout_binding.binding)
						{
							bSkipBinding = true;
							break;
						}
					}
					
					if(!bSkipBinding)
					{
						Bindings[ReflectSet->set].push_back(layout_binding);
					}
				}
			}

			glm::uint32 PushConstantRangeCount = 0;
			spvReflectEnumeratePushConstantBlocks(&ReflectModule, &PushConstantRangeCount, nullptr);

			std::vector<SpvReflectBlockVariable*> ReflectPushConstantRanges(PushConstantRangeCount);
			spvReflectEnumeratePushConstantBlocks(&ReflectModule, &PushConstantRangeCount, ReflectPushConstantRanges.data());

			if (PushConstantRangeCount) {

				for (int i = 0; i < ReflectPushConstantRanges[0]->member_count; i++) {
					auto& member = ReflectPushConstantRanges[0]->members[i];

					int x = 5;
				}
			}

			for (auto& ReflectRange : ReflectPushConstantRanges)
			{
				PushConstantRange.size += ReflectRange->size;
				PushConstantRange.offset = 0;
				PushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
				
			}

			spvReflectDestroyShaderModule(&ReflectModule);
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

		if(Bindings.size())
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
					std::vector<VkDescriptorBindingFlags> BindingFlags;
					for (auto& binding : Bindings[i]) {
						if (binding.descriptorCount != 1) BindingFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
						else BindingFlags.push_back(0);
					}

					VkDescriptorSetLayoutBindingFlagsCreateInfo LayoutBindingsFlags = {};
					LayoutBindingsFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
					LayoutBindingsFlags.bindingCount = BindingFlags.size();
					LayoutBindingsFlags.pBindingFlags = BindingFlags.data();

					VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
					LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					LayoutCreateInfo.pNext = &LayoutBindingsFlags;
					LayoutCreateInfo.bindingCount = Bindings[i].size();
					LayoutCreateInfo.pBindings = Bindings[i].data();

					VkDescriptorSetLayout VkDescriptorSetLayout = VK_NULL_HANDLE;
					vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &VkDescriptorSetLayout);
					SetLayouts.push_back(VkDescriptorSetLayout);
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
		auto Device = FVulkanRenderContext::GetDevice();

		for (auto& Stage : StageCreateInfos)
		{
			if (Stage.module != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(Device, Stage.module, nullptr);
			}
			
		}
		
		for (auto& layout : SetLayouts)
		{
			vkDestroyDescriptorSetLayout(Device, layout, nullptr);
		}

		SetLayouts.clear();
		for (auto& stage : StageCreateInfos)
		{
			stage.module = VK_NULL_HANDLE;
		}
    }
}
