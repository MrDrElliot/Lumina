#pragma once
#include "Assets/Asset.h"
#include "Containers/Array.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectMacros.h"
#include "Renderer/RHIFwd.h"
#include "Material.generated.h"

namespace Lumina
{
    LUM_ENUM()
    enum class EMaterialType : uint8
    {
        None,
        PBR,
        PostProcess,
        UI,
    };

    LUM_CLASS()
    class CMaterial : public CObject
    {
        GENERATED_BODY()
        
    public:


        CMaterial()
        {
            MaterialType = EMaterialType::None;
        }

        TVector<uint8>          GraphData;
        
    private:

        LUM_PROPERTY()
        EMaterialType           MaterialType;

        
        FRHIVertexShaderRef     VertexShader;
        FRHIPixelShaderRef      PixelShader;
        FRHIBindingSetRef       BindingSet;

    };
    
}
