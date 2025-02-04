#include "Mesh.h"

#include "Renderer/RenderTypes.h"


namespace Lumina
{
    void FMesh::PostLoad()
    {
        // After serialization, we want to install the render requirements.
        
        FDeviceBufferSpecification VBOSpec;
        VBOSpec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        VBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        VBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        VBOSpec.Size = sizeof(FVertex) * MeshData.Vertices.size();
        VBOSpec.DebugName = "Vertex Buffer Object";
        
        VBO = FBuffer::Create(VBOSpec, MeshData.Vertices.data(), sizeof(FVertex) * MeshData.Vertices.size());
        VBO->SetFriendlyName("Vertex Buffer Object" + GetAssetPath().GetPathAsString());


        FDeviceBufferSpecification IBOSpec;
        IBOSpec.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IBOSpec.Size = sizeof(uint32) * MeshData.Indices.size();
        IBOSpec.DebugName = "Index Buffer Object";

        IBO = FBuffer::Create(IBOSpec, MeshData.Indices.data(),sizeof(uint32) * MeshData.Indices.size());
        IBO->SetFriendlyName("Index Buffer Object" + GetAssetPath().GetPathAsString());
    }

}
