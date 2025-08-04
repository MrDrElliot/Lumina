#pragma once

#include "Containers/Array.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectMacros.h"
#include "Renderer/RHIFwd.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "MaterialInterface.h"
#include "Renderer/MaterialTypes.h"
#include "Material.generated.h"


namespace Lumina
{
    class CTexture;
}
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
    class LUMINA_API CMaterial : public CMaterialInterface
    {
        GENERATED_BODY()
        
    public:

        CMaterial();


        bool SetScalarValue(const FName& Name, const float Value) override;
        bool SetVectorValue(const FName& Name, const glm::vec4& Value) override;
        bool GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param) override;
        CMaterial* GetMaterial() const override;
        bool IsReadyForRender() const override;
        FRHIBindingSetRef GetBindingSet() const override;
        FRHIBindingLayoutRef GetBindingLayout() const override;
        FRHIVertexShaderRef GetVertexShader() const override;
        FRHIPixelShaderRef GetPixelShader() const override;
        
        LUM_PROPERTY(Editable)
        EMaterialType MaterialType;

        LUM_PROPERTY(Editable)
        bool bCastShadows = true;

        LUM_PROPERTY(Editable)
        bool bTwoSided = false;

        LUM_PROPERTY(Editable)
        bool bTransparent = false;

        
        LUM_PROPERTY()
        TVector<TObjectHandle<CTexture>>        Textures;

        TVector<FMaterialParameter>             Parameters;
        
        FMaterialUniforms                       MaterialUniforms;
        
        FRHIVertexShaderRef                     VertexShader;
        FRHIPixelShaderRef                      PixelShader;
        FRHIBufferRef                           UniformBuffer;
        FRHIBindingLayoutRef                    BindingLayout;
        FRHIBindingSetRef                       BindingSet;

    };
    
}
