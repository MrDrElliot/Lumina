#include "PipelineState.h"

#include "Renderer/RHIIncl.h"


namespace Lumina
{
    void FPipelineState::SetPipeline(const FRHIPipeline& InPipeline)
    {
        // We don't want to rebuild anything if the pipeline hasn't changed.
        if (InPipeline == Pipeline)
        {
            return;
        }
        
        FName ShaderKey = InPipeline->GetSpecification().GetShader();
        
        Pipeline = InPipeline;
        Shader = FShaderLibrary::Get()->GetShader(ShaderKey);
        
        BuildPipelineResources();
    }

    void FPipelineState::BindDescriptors()
    {
        FlushDescriptorWrites();

        for (auto& KVP : DescriptorSets[FRenderer::GetFrameIndex()])
        {
            FRenderer::BindSet(KVP.second, Pipeline, KVP.first, {});
        }
    }

    void FPipelineState::FlushDescriptorWrites()
    {
        while (!PendingDescriptorWrites.empty())
        {
            FPendingDescriptorWrite Write = PendingDescriptorWrites.back();
            
            Write.DescriptorSet->Write(Write.DescriptorSetIndex, 0, Write.Buffer, Write.Buffer->GetSpecification().Size, 0);

            PendingDescriptorWrites.pop();
        }
    }

    void FPipelineState::AddPendingDescriptorWrite(FRHIDescriptorSet Set, FRHIBuffer Buffer, uint8 DescriptorSetIndex)
    {
        PendingDescriptorWrites.emplace(Set, Buffer, DescriptorSetIndex);
    }

    void FPipelineState::ClearState()
    {
        Pipeline = nullptr;
        Shader = nullptr;

        for (auto& Map : DescriptorSets)
        {
            Map.clear();
        }

        for (auto& Map : Buffers)
        {
            Map.clear();
        }
    }

    FPipelineState::FPipelineStateBuffer FPipelineState::GetBufferForDescriptor(const FName& Descriptor)
    {
        Assert(Pipeline);

        auto& Buffer = Buffers[FRenderer::GetFrameIndex()];
        
        auto Itr = Buffer.find(Descriptor);
        Assert(Itr != Buffer.end());
        
        return Itr->second;
        
    }

    FRHIDescriptorSet FPipelineState::GetDescriptorSetForDescriptor(const FName& Descriptor)
    {
        Assert(Pipeline);

        auto& Buffer = Buffers[FRenderer::GetFrameIndex()];
        
        auto Itr = Buffer.find(Descriptor);
        Assert(Itr != Buffer.end());
        
        return DescriptorSets[FRenderer::GetFrameIndex()][Itr->second.DescriptorSetIndex];
        
    }

    void FPipelineState::BuildPipelineResources()
    {
        FShaderReflectionData ReflectionData = Shader->GetShaderReflectionData();
        
        //@ TODO We should not be destroying objects which can be re-used. Such as batching descriptors or buffers that can be slotted.
        //@ TODO Descriptor Cache
        //@ TODO Buffer Cache.
        
        for (auto& Map : DescriptorSets)
        {
            Map.clear();
        }

        for (auto& Map : Buffers)
        {
            Map.clear();
        }

        for (int Frame = 0; Frame < FRAMES_IN_FLIGHT; ++Frame)
        {
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
                    
                        Buffers[Frame].emplace(Binding.Name, Pair);
                    }
                }

                FRHIDescriptorSet DescriptorSet = FDescriptorSet::Create(DescriptorSetSpec);
                DescriptorSets[Frame].emplace(i, DescriptorSet);
            }
        }
    }
}
