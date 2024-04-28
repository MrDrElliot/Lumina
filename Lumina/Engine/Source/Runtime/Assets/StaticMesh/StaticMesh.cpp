#include "StaticMesh.h"

#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Buffer.h"

namespace Lumina
{
    LStaticMesh::LStaticMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo): LAsset(Metadata)
    {

        MeshData = InInfo;
        FDeviceBufferSpecification VBOSPec;
        VBOSPec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        VBOSPec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        VBOSPec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        VBOSPec.Size = sizeof(FVertex) * InInfo.Vertices.size();
        
        VBO = FBuffer::Create(VBOSPec, InInfo.Vertices.data(), sizeof(FVertex) * InInfo.Vertices.size());

        FDeviceBufferSpecification IBOSPec;
        IBOSPec.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IBOSPec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IBOSPec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IBOSPec.Size = sizeof(uint32_t) * InInfo.Indices.size();

        IBO = FBuffer::Create(IBOSPec, InInfo.Indices.data(),sizeof(uint32_t) * InInfo.Indices.size());
    }

    std::shared_ptr<LStaticMesh> LStaticMesh::CreateMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo)
    {
        return std::make_shared<LStaticMesh>(Metadata, InInfo);
    }
}
