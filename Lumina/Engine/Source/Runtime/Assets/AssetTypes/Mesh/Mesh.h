#pragma once

#include "Core/Object/Object.h"
#include "Renderer/MeshData.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Object/Cast.h"
#include "Mesh.generated.h"

namespace Lumina
{
    class CMaterial;
}

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CMesh : public CObject
    {
        GENERATED_BODY()

        friend class CStaticMeshFactory;
        
    public:

        void Serialize(FArchive& Ar) override;
        void Serialize(IStructuredArchive::FSlot Slot) override;

        
        FORCEINLINE const FMeshResource& GetMeshResource() const { return MeshResource; }
        FORCEINLINE const uint32 GetNumVertices() const { return (uint32)MeshResource.Vertices.size(); }
        FORCEINLINE const uint32 GetNumIndicies() const { return (uint32)MeshResource.Indices.size(); }
        
        INLINE CMaterial* GetMaterial() { return Cast<CMaterial>(Material); }
        INLINE const FRHIBufferRef GetVertexBuffer() const { return VertexBuffer; }
        INLINE const FRHIBufferRef GetIndexBuffer() const { return IndexBuffer; }
        
        void PostLoad() override;
        

        LUM_PROPERTY()
        CObject*        Material;
        
    private:
        
        FMeshResource   MeshResource;
        FRHIBufferRef   VertexBuffer;
        FRHIBufferRef   IndexBuffer;
    };
    
}
