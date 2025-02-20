#include "BatchedElements.h"

#include "Buffer.h"
#include "PipelineLibrary.h"
#include "Renderer.h"
#include "RenderTypes.h"
#include "RHIFwd.h"

namespace Lumina
{

    bool FBatchedElements::Draw(const FViewport& ViewVolume)
    {
        if (LineVertices.empty())
        {
            return false;
        }

        glm::mat4 ViewProjectionMatrix = ViewVolume.GetViewVolume().GetViewProjectionMatrix();
        float ViewportSizeX = ViewVolume.GetSize().X;
        float ViewportSizeY = ViewVolume.GetSize().Y;

        uint32 MaxVertices = 0;

        size_t VertexCount = LineVertices.size();
        size_t BufferSize = sizeof(FSimpleElementVertex) * VertexCount;

        FDeviceBufferSpecification BufferSpec;
        BufferSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        BufferSpec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        BufferSpec.Size = BufferSize;
        FRHIBuffer Buffer = FBuffer::Create(BufferSpec);        
        Buffer->SetFriendlyName("Lines");

        Buffer->UploadData(0, LineVertices.data(), BufferSize);

        FPipelineSpec PipelineSpec = FPipelineSpec::Create()
        .SetPrimitiveTopology(EPipelineTopology::LINES)
        .SetPolygonFillMode(EPipelineFillMode::EDGE_ONLY)
        .SetVertexBinding<FSimpleElementVertex>()
        .SetShader("Primitive");
        
        FRHIPipeline Pipeline = FPipelineLibrary::Get()->GetOrCreatePipeline(PipelineSpec);
        
        FRenderer::BindPipeline(Pipeline);

        FRenderer::BindVertexBuffer(Buffer);
    
        FRenderer::DrawVertices(VertexCount);

        return true;
    }

}
