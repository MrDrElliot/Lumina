#include "MaterialInstance.h"
#include "Renderer/RHIIncl.h"
#include "Material.h"


namespace Lumina
{
    CMaterial* CMaterialInstance::GetMaterial() const
    {
        return Material.Get();
    }

    bool CMaterialInstance::SetScalarValue(const FName& Name, const float Value)
    {
        if (!Material.IsValid())
        {
            return false;
        }

        return Material->SetScalarValue(Name, Value);
    }

    bool CMaterialInstance::SetVectorValue(const FName& Name, const glm::vec4& Value)
    {
        if (!Material.IsValid())
        {
            return false;
        }

        return Material->SetVectorValue(Name, Value);
    }

    bool CMaterialInstance::GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param)
    {
        if (!Material.IsValid())
        {
            return false;
        }

        return Material->GetParameterValue(Type, Name, Param);
    }

    bool CMaterialInstance::IsReadyForRender() const
    {
        if (!Material.IsValid())
        {
            return false;
        }

        return Material->IsReadyForRender();
    }
}
