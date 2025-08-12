#include "MaterialInstance.h"
#include "Renderer/RHIIncl.h"
#include "Material.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Engine/Engine.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RHIGlobals.h"


namespace Lumina
{
    CMaterialInstance::CMaterialInstance()
    {
        Memory::Memzero(&MaterialUniforms, sizeof(FMaterialUniforms));
        if (Material)
        {
            MaterialUniforms = Material->MaterialUniforms;
            Parameters = Material->Parameters;
        }
    }

    CMaterial* CMaterialInstance::GetMaterial() const
    {
        return Material.Get();
    }

    bool CMaterialInstance::SetScalarValue(const FName& Name, const float Value)
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

    bool CMaterialInstance::SetVectorValue(const FName& Name, const glm::vec4& Value)
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

    bool CMaterialInstance::GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param)
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

    bool CMaterialInstance::IsReadyForRender() const
    {
        if (!Material.IsValid())
        {
            return false;
        }

        return Material->IsReadyForRender();
    }

    FRHIBindingSetRef CMaterialInstance::GetBindingSet() const
    {
        return BindingSet;
    }

    FRHIBindingLayoutRef CMaterialInstance::GetBindingLayout() const
    {
        return Material->GetBindingLayout();
    }

    FRHIVertexShaderRef CMaterialInstance::GetVertexShader() const
    {
        return Material->GetVertexShader();
    }

    FRHIPixelShaderRef CMaterialInstance::GetPixelShader() const
    {
        return Material->GetPixelShader();
    }

    void CMaterialInstance::PostLoad()
    {
        if (Material)
        {
            MaterialUniforms = Material->MaterialUniforms;
            Parameters = Material->Parameters;
            
            FRHIBufferDesc BufferDesc;
            BufferDesc.Size = sizeof(FMaterialUniforms);
            BufferDesc.DebugName = "Material Uniforms";
            BufferDesc.InitialState = EResourceStates::ConstantBuffer;
            BufferDesc.bKeepInitialState = true;
            BufferDesc.Usage.SetFlag(BUF_UniformBuffer);
            UniformBuffer = GRenderContext->CreateBuffer(BufferDesc);
            GRenderContext->SetObjectName(UniformBuffer, GetName().c_str(), EAPIResourceType::Buffer);


            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::BufferCBV(0, UniformBuffer));

            for (SIZE_T i = 0; i < Material->Textures.size(); ++i)
            {
                FRHIImageRef Image = Material->Textures[i]->RHIImage;
                
                SetDesc.AddItem(FBindingSetItem::TextureSRV((uint32)i, Image));
            }

            GRenderContext->GetCommandList(ECommandQueue::Graphics)->WriteBuffer(UniformBuffer, &MaterialUniforms, 0, sizeof(FMaterialUniforms));

            BindingSet = GRenderContext->CreateBindingSet(SetDesc, Material->BindingLayout);

        }
    }
}
