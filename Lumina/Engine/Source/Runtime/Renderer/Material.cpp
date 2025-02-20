#include "Material.h"
#include "Renderer/RHIIncl.h"


namespace Lumina
{
    AMaterial::AMaterial()
    {
    }

    AMaterial::~AMaterial()
    {
        
    }
    

    void AMaterial::Bind()
    {
        FRenderer::BindPipeline(QueryPipeline());
    }

    TRefCountPtr<FPipeline> AMaterial::QueryPipeline()
    {
        if (Pipeline == nullptr)
        {
            //Pipeline = FPipelineLibrary::Get()->GetOrCreatePipeline(MaterialSpec);
        }
        
        return Pipeline;
    }

    TRefCountPtr<FShader> AMaterial::QueryShader() const
    {
        return nullptr; //FShaderLibrary::Get()->GetShader(MaterialSpec.GetShader());
    }
}
