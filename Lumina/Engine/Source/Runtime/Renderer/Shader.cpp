#include "Shader.h"

#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanShader.h"


namespace Lumina
{
    FRHIShader FShader::Create()
    {
        return FRHIShader(MakeRefCount<FVulkanShader>());
    }

    void FShader::GenerateShaderStageReflectionData(const FShaderStage& StageData, SpvReflectShaderModule* ReflectionModule)
    {

        //---------------------------------------------------------------------------
        // Enumerate Input Bindings
        
        {
            uint32 InputBindingCount = 0;
            spvReflectEnumerateInputVariables(ReflectionModule, &InputBindingCount, nullptr);

            TVector<SpvReflectInterfaceVariable*> InputBindings(InputBindingCount);
            spvReflectEnumerateInputVariables(ReflectionModule, &InputBindingCount, InputBindings.data());

            for (const SpvReflectInterfaceVariable* InputBinding : InputBindings)
            {
                FInputVariableBinding InputVariable = {};
                InputVariable.Name = InputBinding->name;
                InputVariable.Count = InputBinding->member_count;
                InputVariable.Location = InputBinding->location;
                InputVariable.Size = (InputBinding->numeric.scalar.width * InputBinding->numeric.vector.component_count) / 8;
            
                ShaderReflectionData.InputVariableBindings.push_back(InputVariable);
            }
        }
        
        //---------------------------------------------------------------------------
        // Enumerate Descriptor Sets.
        
        {
            uint32 DescriptorSetCount = 0;
            spvReflectEnumerateDescriptorSets(ReflectionModule, &DescriptorSetCount, nullptr);
		
            TVector<SpvReflectDescriptorSet*> ReflectDescriptorSets(DescriptorSetCount);
            spvReflectEnumerateDescriptorSets(ReflectionModule, &DescriptorSetCount, ReflectDescriptorSets.data());

            for (SpvReflectDescriptorSet* ReflectSet : ReflectDescriptorSets)
            {
                THashMap<FName, FDescriptorBinding> SetDescriptorBindings;

                for (uint32 i = 0; i < ReflectSet->binding_count; i++)
                {
                    SpvReflectDescriptorBinding* ReflectBinding = ReflectSet->bindings[i];
		
                    FName DescriptorBindingKey = FName(ReflectBinding->name);
                    if (SetDescriptorBindings.find(DescriptorBindingKey) == SetDescriptorBindings.end())
                    {
                        FDescriptorBinding DescriptorBinding;
                        DescriptorBinding.Name = DescriptorBindingKey;
                        DescriptorBinding.Set = ReflectBinding->set;
                        DescriptorBinding.Binding = ReflectBinding->binding;
                        DescriptorBinding.ArrayCount = ReflectBinding->count;
                        DescriptorBinding.Type = (EDescriptorBindingType)ReflectBinding->descriptor_type;
                        DescriptorBinding.Size = ReflectBinding->count * ReflectBinding->block.size;

                        SetDescriptorBindings.emplace(DescriptorBindingKey, DescriptorBinding);
                    }

                    SetDescriptorBindings[DescriptorBindingKey].StageFlags |= (uint32)StageData.Stage;

                    if (ReflectBinding->count > 1)
                    {
                        SetDescriptorBindings[DescriptorBindingKey].Flags = (uint64)EDescriptorFlags::PARTIALLY_BOUND;
                    }

                    ShaderReflectionData.DescriptorBindings.push_back(SetDescriptorBindings);

                }
            }
        }


        //---------------------------------------------------------------------------
        // Enumerate Push Constant Blocks
        
        {
            uint32 PushConstantRangeCount = 0;
            spvReflectEnumeratePushConstantBlocks(ReflectionModule, &PushConstantRangeCount, nullptr);
            
            TVector<SpvReflectBlockVariable*> ReflectPushConstantRanges(PushConstantRangeCount);
            spvReflectEnumeratePushConstantBlocks(ReflectionModule, &PushConstantRangeCount, ReflectPushConstantRanges.data());
            
            for (SpvReflectBlockVariable* ReflectRange : ReflectPushConstantRanges)
            {
                FName PushConstantKey = FName(ReflectRange->name);
                if (ShaderReflectionData.PushConstantRanges.find(PushConstantKey) == ShaderReflectionData.PushConstantRanges.end())
                {
                    FPushConstantRange PushConstantRange;
                    PushConstantRange.Name = PushConstantKey;
                    PushConstantRange.Size = ReflectRange->size;
                    PushConstantRange.PaddedSize = ReflectRange->padded_size;
                    PushConstantRange.AbsoluteOffset = ReflectRange->absolute_offset;
                    PushConstantRange.Offset = ReflectRange->offset;
            
                    for (uint32 i = 0; i < ReflectRange->member_count; ++i)
                    {
                        SpvReflectBlockVariable& Member = ReflectRange->members[i];
            
                        FPushConstantMember PushConstantMember;
                        PushConstantMember.Name = FName(Member.name);
                        PushConstantMember.Size = Member.size;
                        PushConstantMember.Offset = Member.offset;
                        
                        PushConstantRange.Members.push_back(PushConstantMember);
                    }

                    ShaderReflectionData.PushConstantRanges.emplace(PushConstantKey, PushConstantRange);
                }
                    
                ShaderReflectionData.PushConstantRanges[PushConstantKey].StageFlags |= (uint32)StageData.Stage;

            }
        }
    }

    void FShader::PrintShaderReflectionData()
    {
        std::ostringstream ss;

        // Descriptor Bindings
        ss << "Descriptor Bindings:\n";
        for (size_t i = 0; i < ShaderReflectionData.DescriptorBindings.size(); ++i)
        {
            const auto& descriptorMap = ShaderReflectionData.DescriptorBindings[i];
            for (const auto& [name, binding] : descriptorMap)
            {
                ss << "  - Name: " << name.c_str() << "\n"
                   << "    Set: " << binding.Set << "\n"
                   << "    Binding: " << binding.Binding << "\n"
                   << "    Type: " << static_cast<int>(binding.Type) << "\n"
                   << "    Array Count: " << binding.ArrayCount << "\n"
                   << "    Size: " << binding.Size << " bytes\n"
                   << "    Flags: " << binding.Flags << "\n"
                   << "    Stage Flags: " << binding.StageFlags << "\n\n";
            }
        }

        // Push Constant Ranges
        ss << "Push Constant Ranges:\n";
        for (const auto& [name, range] : ShaderReflectionData.PushConstantRanges)
        {
            ss << "  - Name: " << name.c_str() << "\n"
               << "    Offset: " << range.Offset << "\n"
               << "    Size: " << range.Size << " bytes\n"
               << "    Padded Size: " << range.PaddedSize << "\n\n";
        }

        LOG_DEBUG("Shader Reflection Data: {0}", ss.str());
    }
}
