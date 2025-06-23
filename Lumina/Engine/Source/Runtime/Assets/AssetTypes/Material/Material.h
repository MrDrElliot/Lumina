#pragma once

#include "Containers/Array.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectMacros.h"
#include "Renderer/RHIFwd.h"
#include "Core/Object/ObjectPtr.h"
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
        
        
        LUM_PROPERTY(Editable, Category = "Graph")
        TVector<CObject*> MaterialNodes;

        LUM_PROPERTY()
        TVector<uint16> Connections;
        
        LUM_PROPERTY()
        EMaterialType MaterialType;
        
        
        FRHIVertexShaderRef     VertexShader;
        FRHIPixelShaderRef      PixelShader;
        FRHIBindingSetRef       BindingSet;

    };
    
}
