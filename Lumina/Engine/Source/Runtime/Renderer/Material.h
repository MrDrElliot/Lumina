#pragma once

#include <glm/glm.hpp>
#include "Memory/RefCounted.h"
#include "Image.h"
#include "Assets/Asset.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FBuffer;
    class FPipeline;
    
    class FDescriptorSet;
    
    class FMaterial : public FRenderResource
    {
    public:

        FMaterial(const TRefPtr<FPipeline>& InPipeline);
        ~FMaterial();

        static TRefPtr<FMaterial> Create(const TRefPtr<FPipeline>& InPipeline);
        
        void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FImage> Image, TRefPtr<FImageSampler> Sampler);
        void Write(uint16 Binding, uint16 ArrayElement, TRefPtr<FBuffer> Buffer, uint64 Size, uint64 Offset);
        
        virtual void Bind(const TRefPtr<FPipeline>& Pipeline, uint32 Index = 0);


    private:

        TRefPtr<FPipeline> Pipeline;
        TArray<TRefPtr<FDescriptorSet>> DescriptorSets;
    };
}
