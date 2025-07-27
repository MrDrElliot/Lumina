#pragma once

#include "Containers/Array.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectMacros.h"
#include "Renderer/RHIFwd.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Material.generated.h"


namespace Lumina
{
    class CTexture;
}

#define MAX_VEC4 24
#define MAX_SCALARS 24

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

    struct FMaterialUniforms
    {
        glm::vec4 Scalars[MAX_SCALARS / 4];
        glm::vec4 Vec4s[MAX_VEC4];
    };

    LUM_CLASS()
    class LUMINA_API CMaterial : public CObject
    {
        GENERATED_BODY()
        
    public:

        CMaterial();
        
        LUM_PROPERTY(Editable)
        EMaterialType MaterialType;

        
        LUM_PROPERTY()
        TVector<TObjectHandle<CTexture>>        Textures;

        FMaterialUniforms                       MaterialUniforms;
        FRHIVertexShaderRef                     VertexShader;
        FRHIPixelShaderRef                      PixelShader;
        FRHIBufferRef                           UniformBuffer;
        FRHIBindingSetRef                       BindingSet;
        FRHIBindingLayoutRef                    BindingLayout;
        FRHIGraphicsPipelineRef                 Pipeline;

    };
    
}
