#include "Mesh.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderTypes.h"


namespace Lumina
{
    void AMesh::PostLoad()
    {
        // After serialization, we want to install the render requirements.
        
        FDeviceBufferSpecification VBOSpec;
        VBOSpec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        VBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        VBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        VBOSpec.Size = sizeof(FVertex) * MeshResource.Vertices.size();
        
        MeshResource.VBO = FBuffer::Create(VBOSpec, MeshResource.Vertices.data(), sizeof(FVertex) * MeshResource.Vertices.size());
        MeshResource.VBO->SetFriendlyName("Vertex Buffer Object" + GetAssetPath().GetPathAsString());


        FDeviceBufferSpecification IBOSpec;
        IBOSpec.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IBOSpec.Size = sizeof(uint32) * MeshResource.Indices.size();

        MeshResource.IBO = FBuffer::Create(IBOSpec, MeshResource.Indices.data(),sizeof(uint32) * MeshResource.Indices.size());
        MeshResource.IBO->SetFriendlyName("Index Buffer Object" + GetAssetPath().GetPathAsString());
    }

    void AMesh::Bind()
    {
        Assert(MeshResource.GetVertexBuffer() != nullptr);
        Assert(MeshResource.GetIndexBuffer() != nullptr);
        
        FRenderer::BindVertexBuffer(MeshResource.GetVertexBuffer());
        FRenderer::BindIndexBuffer(MeshResource.GetIndexBuffer());
    }
}
