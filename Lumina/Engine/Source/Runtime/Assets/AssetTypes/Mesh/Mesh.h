#pragma once

#include "Core/Object/Object.h"
#include "Renderer/MeshData.h"
#include "Mesh.generated.h"

namespace Lumina
{
    class CMaterial;
    LUM_CLASS()
    class LUMINA_API CMesh : public CObject
    {
        GENERATED_BODY()
        friend class CStaticMeshFactory;
    public:
        
        
        void Serialize(FArchive& Ar) override;
        void Serialize(IStructuredArchive::FSlot Slot) override;
        void PostLoad() override;

        bool IsReadyForRender() const;

        uint32 GetNumMaterials() const { return Materials.size(); }
        CMaterial* GetMaterialAtSlot(SIZE_T Slot) const;
        const FGeometrySurface& GetSurface(SIZE_T Slot) { return MeshResources.GeometrySurfaces[Slot]; }
        const FMeshResource& GetMeshResource() const { return MeshResources; }
        
        uint32 GetNumVertices() const { return MeshResources.Vertices.size(); }
        uint32 GetNumIndices() const { return MeshResources.Indices.size(); }
        
        LUM_PROPERTY(Editable, Category = "Graph")
        TVector<CMaterial*> Materials;
    
    private:
        
        FMeshResource MeshResources;
    };
    
}
