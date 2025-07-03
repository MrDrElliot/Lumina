#pragma once

#include "Core/Object/Object.h"
#include "Memory/RefCounted.h"
#include "Renderer/RenderResource.h"
#include "Texture.generated.h"

namespace Lumina
{

    LUM_CLASS()
    class LUMINA_API CTexture : public CObject
    {
        GENERATED_BODY()
        
    public:

        void Serialize(FArchive& Ar) override;
        void Serialize(IStructuredArchive::FSlot Slot) override;
        void PostLoad() override;
        
        
        FRHIImageDesc   ImageDescription;
        TVector<uint8>  Pixels;
        FRHIImageRef    RHIImage;
 
    };
}
