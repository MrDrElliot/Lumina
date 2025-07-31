
#include "Material.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{
    CMaterial::CMaterial()
    {
        MaterialType = EMaterialType::PBR;
        Memory::Memzero(&MaterialUniforms, sizeof(FMaterialUniforms));
    }

    bool CMaterial::SetScalarValue(const FName& Name, const float Value)
    {
        auto* Itr = eastl::find_if(Parameters.begin(), Parameters.end(), [Name](const FMaterialParameter& Param)
        {
           return Param.ParameterName == Name && Param.Type == EMaterialParameterType::Scalar; 
        });

        if (Itr != Parameters.end())
        {
            const FMaterialParameter& Param = *Itr;

            int CecIndex = Param.Index / 4;
            int ComponentIndex = Param.Index % 4;
            
            MaterialUniforms.Scalars[CecIndex][ComponentIndex] = Value;
            return true;
        }
        else
        {
            LOG_ERROR("Failed to find material scalar parameter {}", Name);
        }

        return false;
    }

    bool CMaterial::SetVectorValue(const FName& Name, const glm::vec4& Value)
    {
        auto* Itr = eastl::find_if(Parameters.begin(), Parameters.end(), [Name](const FMaterialParameter& Param)
        {
           return Param.ParameterName == Name && Param.Type == EMaterialParameterType::Vector; 
        });

        if (Itr != Parameters.end())
        {
            const FMaterialParameter& Param = *Itr;
            MaterialUniforms.Vectors[Param.Index] = Value;
            return true;
        }
        else
        {
            LOG_ERROR("Failed to find material vector parameter {}", Name);
        }

        return false;
    }

    bool CMaterial::GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param)
    {
        Param = {};
        auto* Itr = eastl::find_if(Parameters.begin(), Parameters.end(), [Type, Name](const FMaterialParameter& Param)
        {
           return Param.ParameterName == Name && Param.Type == Type; 
        });

        if (Itr != Parameters.end())
        {
            Param = *Itr;
            return true;
        }
        
        return false;
    }

    CMaterial* CMaterial::GetMaterial() const
    {
        return const_cast<CMaterial*>(this);
    }

    bool CMaterial::IsReadyForRender() const
    {
        return VertexShader != nullptr && PixelShader != nullptr && BindingLayout != nullptr && BindingSet != nullptr;
    }
}
