#pragma once

#include "Core/Object/Object.h"
#include "Renderer/MeshData.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Mesh.generated.h"

namespace Lumina
{
    class CMaterialInterface;
    class CMaterialInstance;
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
        void PostLoad() override;

        bool IsReadyForRender() const;

        uint32 GetNumMaterials() const { return (uint32)Materials.size(); }
        CMaterialInterface* GetMaterialAtSlot(SIZE_T Slot) const;
        void SetMaterialAtSlot(SIZE_T Slot, CMaterialInterface* NewMaterial);
        
        const FGeometrySurface& GetSurface(SIZE_T Slot) { return MeshResources.GeometrySurfaces[Slot]; }
        const FMeshResource& GetMeshResource() const { return MeshResources; }

        void SetMeshResource(const FMeshResource& NewResource);
        
        SIZE_T GetNumVertices() const { return MeshResources.Vertices.size(); }
        SIZE_T GetNumIndices() const { return MeshResources.Indices.size(); }
        
        LUM_PROPERTY(Editable, Category = "Materials")
        TVector<TObjectHandle<CMaterialInterface>> Materials;
    
    private:
        
        FMeshResource MeshResources;
    };
    
}
