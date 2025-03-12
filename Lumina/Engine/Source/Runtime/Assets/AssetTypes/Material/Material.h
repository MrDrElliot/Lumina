#pragma once
#include "Assets/Asset.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    
    class AMaterial : public IAsset
    {
    public:

        enum class EType : uint8
        {
            None,
            PBR,
            PostProcess,
            UI,
        };

        DECLARE_ASSET("Material", Material, 1.0)
        
        
    private:

        EType                   MaterialType = EType::None;
        FRHIVertexShaderRef     VertexShader;
        FRHIPixelShaderRef      PixelShader;
        FRHIBindingSetRef       BindingSet;
    };
    
}
