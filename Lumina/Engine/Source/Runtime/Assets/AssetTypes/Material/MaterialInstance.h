#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "MaterialInterface.h"
#include "MaterialInstance.generated.h"

namespace Lumina { class CMaterial; };

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CMaterialInstance : public CMaterialInterface
    {
        GENERATED_BODY()
    public:
        
        CMaterial* GetMaterial() const override;
        bool SetScalarValue(const FName& Name, const float Value) override;
        bool SetVectorValue(const FName& Name, const glm::vec4& Value) override;
        bool GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param) override;
        bool IsReadyForRender() const override;

        LUM_PROPERTY(ReadOnly, Category = "Material")
        TObjectHandle<CMaterial> Material;
    };
}
