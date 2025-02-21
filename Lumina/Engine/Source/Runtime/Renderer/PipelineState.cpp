#include "PipelineState.h"

#include "Renderer/RHIIncl.h"


namespace Lumina
{
    void FPipelineState::SetPipeline(const FRHIPipeline& InPipeline)
    {
        if (InPipeline == Pipeline)
        {
            return;
        }
        
        FName ShaderKey = InPipeline->GetSpecification().GetShader();
        
        Pipeline = InPipeline;
        Shader = FShaderLibrary::Get()->GetShader(ShaderKey);
        
        BuildPipelineResources();
        
    }

    void FPipelineState::ClearState()
    {
        Pipeline = nullptr;
        Shader = nullptr;
        DescriptorSets.clear();
        Buffers.clear();
    }

    FPipelineState::FPipelineStateBuffer FPipelineState::GetBufferForDescriptor(const FName& Descriptor)
    {
        Assert(Pipeline);
        
        auto Itr = Buffers.find(Descriptor);
        Assert(Itr != Buffers.end());
        
        return Itr->second;
        
    }

    FRHIDescriptorSet FPipelineState::GetDescriptorSetForDescriptor(const FName& Descriptor)
    {
        Assert(Pipeline);
        
        auto Itr = Buffers.find(Descriptor);
        Assert(Itr != Buffers.end());
        
        return DescriptorSets[Itr->second.DescriptorSetIndex];
        
    }

    void FPipelineState::BuildPipelineResources()
    {
        FShaderReflectionData ReflectionData = Shader->GetShaderReflectionData();

        DescriptorSets.clear();
        Buffers.clear();

        /** The first teration represents an entire descritptor set. */
        for (uint64 i = 0; i < ReflectionData.DescriptorBindings.size(); ++i)
        {
            const auto& DescriptorHashMaps = ReflectionData.DescriptorBindings[i];
            
            FDescriptorSetSpecification DescriptorSetSpec = {};
            
            /** This iteration represents each descriptor binding within that set */

            for (const auto& KVP : DescriptorHashMaps)
            {
                const FDescriptorBinding& Binding = KVP.second;
                DescriptorSetSpec.Bindings.push_back(Binding);

                if (Binding.Type == EDescriptorBindingType::UNIFORM_BUFFER)
                {
                    FDeviceBufferSpecification BufferSpec;
                    BufferSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
                    BufferSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
                    BufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
                    BufferSpec.Size = Binding.Size;
                    
                    FRHIBuffer UniformBuffer = FBuffer::Create(BufferSpec);
                    UniformBuffer->SetFriendlyName("Unifrom Buffer");
                    
                    FPipelineStateBuffer Pair;
                    Pair.DescriptorSetIndex = i;
                    Pair.DescriptorIndex = KVP.second.Binding;
                    Pair.Buffer = UniformBuffer;
                    
                    Buffers.emplace(Binding.Name, Pair);
                }
            }

            FRHIDescriptorSet DescriptorSet = FDescriptorSet::Create(DescriptorSetSpec);
            DescriptorSets.emplace(i, DescriptorSet);
        }
    }
}
