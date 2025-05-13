#pragma once

#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Core/Object/Object.h"
#include "Renderer/MeshData.h"
#include "Mesh.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CMesh : public CObject
    {
        GENERATED_BODY()
        
    public:
        
        FORCEINLINE const FMeshResource& GetMeshResource() const { return MeshResource; }
        FORCEINLINE const uint32 GetNumIndicies() const { return (uint32)MeshResource.Indices.size(); }
        FORCEINLINE const uint32 GetNumVertices() const { return (uint32)MeshResource.Indices.size(); }

        void Bind();

        
    private:
        
        FMeshResource                               MeshResource = {};
    };
    
}
