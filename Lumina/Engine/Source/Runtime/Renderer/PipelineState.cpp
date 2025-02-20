#include "PipelineState.h"

#include "Renderer/RHIIncl.h"


namespace Lumina
{
    void FPipelineState::SetPipeline(const FRHIPipeline& InPipeline)
    {
        FName ShaderKey = InPipeline->GetSpecification().GetShader();
        
        Pipeline = InPipeline;
        Shader = FShaderLibrary::GetShader(ShaderKey);
        
        BuildPipelineResources();
        
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
                    BufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
                    BufferSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
                    BufferSpec.Size = Binding.Size;
                    
                    FRHIBuffer UniformBuffer = FBuffer::Create(BufferSpec);
                    TPair<FName, FRHIBuffer> Pair;
                    Pair.first = Binding.Name;
                    Pair.second = UniformBuffer;
                    
                    Buffers.emplace(i, Pair);
                }
            }

            FRHIDescriptorSet DescriptorSet = FDescriptorSet::Create(DescriptorSetSpec);
            DescriptorSets.emplace(i, DescriptorSet);
        }
    }
}
