#include "BatchedElements.h"

#include "PipelineLibrary.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "RenderTypes.h"
#include "RHIIncl.h"

namespace Lumina
{
    FBatchedElements::FBatchedElements()
    {
    }

    FBatchedElements::~FBatchedElements()
    {
    }

    void FBatchedElements::Initialize()
    {
        FDeviceBufferSpecification BufferSpec;
        BufferSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        BufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        BufferSpec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        BufferSpec.Size = sizeof(FSimpleElementVertex) * 1024;
        VertexBuffer = FRenderer::GetRenderContext()->CreateBuffer(BufferSpec);        
        VertexBuffer->SetFriendlyName("Lines");
    }

    void FBatchedElements::SubmitElement(const FSimpleElementVertex& Element)
    {
        LineVertices.push_back(Element);
    }

    bool FBatchedElements::Draw(const FSceneGlobalData& GlobalData)
    {
        if (LineVertices.empty())
        {
            return false;
        }
        
        uint32 MaxVertices = 0;

        size_t VertexCount = LineVertices.size();
        size_t BufferSize = sizeof(FSimpleElementVertex) * VertexCount;

        VertexBuffer->UploadData(0, LineVertices.data(), BufferSize);
        
        FPipelineSpec PipelineSpec = FPipelineSpec::Create()
        .SetPrimitiveTopology(EPipelineTopology::TRIANGLES)
        .SetPolygonFillMode(EPipelineFillMode::FILL)
        .SetLineWidth(1.0f)
        .SetShader("Primitive.glsl");
        
        FRHIPipeline Pipeline = FPipelineLibrary::Get()->GetOrCreatePipeline(PipelineSpec);
        
        FRenderer::BindPipeline(Pipeline);

        FSceneGlobalData Data = GlobalData;
        FRenderer::SetShaderParameter("SceneUBO", &Data, sizeof(FSceneGlobalData));
        
        FRenderer::BindVertexBuffer(VertexBuffer);
    
        FRenderer::DrawVertices(6);

        LineVertices.clear();
        
        return true;
    }

}
