#pragma once

#include "RenderResource.h"
#include "RHIFwd.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FShaderParameter
    {
    public:

        enum class EType : uint8
        {
            Texture,
            UniformBuffer,
            Max,
        };

        FShaderParameter(EType InType, FRHIResource InResource, uint16 InIndex)
            : Type(InType)
            , RHIResource(InResource)
            , Index(InIndex)
        {}

        FShaderParameter(FRHIImage Image, uint16 InIndex)
            : Type(EType::Texture)
            , RHIResource(Image)
            , Index(InIndex)
        {}

        FShaderParameter(FRHIBuffer Buffer, uint16 InIndex)
            : Type(EType::UniformBuffer)
            , RHIResource(Buffer)
            , Index(InIndex)
        {}

    private:

        FRHIResource    RHIResource = nullptr;
        EType           Type = EType::Max;
        uint16          Index = 0;
        
    };
}
