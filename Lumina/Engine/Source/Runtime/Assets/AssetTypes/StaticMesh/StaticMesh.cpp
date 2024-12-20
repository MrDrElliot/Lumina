#include "StaticMesh.h"

#include "Core/Assertions/Assert.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Buffer.h"

namespace Lumina
{


    LStaticMesh::LStaticMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo)
    : LAsset(Metadata), MeshData(std::move(InInfo))
    {
        
        FDeviceBufferSpecification VBOSpec;
        VBOSpec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        VBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        VBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        VBOSpec.Size = sizeof(FVertex) * MeshData.Vertices.size();
        VBOSpec.DebugName = "Vertex Buffer Object";
        
        VBO = FBuffer::Create(VBOSpec, MeshData.Vertices.data(), sizeof(FVertex) * MeshData.Vertices.size());

        FDeviceBufferSpecification IBOSpec;
        IBOSpec.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IBOSpec.Size = sizeof(uint32) * MeshData.Indices.size();
        IBOSpec.DebugName = "Index Buffer Object";

        IBO = FBuffer::Create(IBOSpec, MeshData.Indices.data(),sizeof(uint32) * MeshData.Indices.size());
    }

    std::shared_ptr<LStaticMesh> LStaticMesh::CreateMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo)
    {
        AssertMsg(InInfo.Vertices.data(), "Attempted to create a static mesh with invalid vertices");
        AssertMsg(InInfo.Indices.data(), "Attempted to create a static mesh with invalid indices");

        return std::make_shared<LStaticMesh>(Metadata, std::move(InInfo));
    }

    void LStaticMesh::CreateNew()
    {
        FDeviceBufferSpecification VBOSpec;
        VBOSpec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        VBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        VBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        VBOSpec.Size = sizeof(FVertex) * MeshData.Vertices.size();
        VBOSpec.DebugName = "Vertex Buffer Object";
        
        VBO = FBuffer::Create(VBOSpec, MeshData.Vertices.data(), sizeof(FVertex) * MeshData.Vertices.size());

        FDeviceBufferSpecification IBOSpec;
        IBOSpec.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IBOSpec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IBOSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IBOSpec.Size = sizeof(uint32) * MeshData.Indices.size();
        IBOSpec.DebugName = "Index Buffer Object";

        IBO = FBuffer::Create(IBOSpec, MeshData.Indices.data(),sizeof(uint32) * MeshData.Indices.size());
    }

    void LStaticMesh::Destroy()
    {
        if(IBO)
        {
            IBO->Destroy();
        }

        if(VBO)
        {
            VBO->Destroy();
        }
    }

    void LStaticMesh::Serialize(FArchive& Ar)
    {
        LAsset::Serialize(Ar);

        Ar << MeshData;
    }
}
