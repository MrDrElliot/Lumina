#include "Mesh.h"

#include "Renderer/RenderContext.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "assets/assettypes/material/materialinstance.h"
#include "Core/Object/Cast.h"


namespace Lumina
{
    void CMesh::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
        Ar << MeshResources;
    }

    void CMesh::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CMesh::PostLoad()
    {
        
    }

    CMaterialInterface* CMesh::GetMaterialAtSlot(SIZE_T Slot) const
    {
        return Materials.empty() ? nullptr : Cast<CMaterialInterface>(Materials[Slot].Get());
    }

    void CMesh::SetMaterialAtSlot(SIZE_T Slot, CMaterialInterface* NewMaterial)
    {
        if (Materials.size() <= Slot)
        {
            Materials.push_back(NewMaterial);
        }
        else
        {
            Materials[Slot] = NewMaterial;
        }  
    }

    void CMesh::SetMeshResource(const FMeshResource& NewResource)
    {
        MeshResources = NewResource;
    }

    bool CMesh::IsReadyForRender() const
    {
        if (HasAnyFlag(OF_NeedsLoad) || HasAllFlags(OF_MarkedGarbage))
        {
            return false;
        }

        for (CMaterialInterface* Material : Materials)
        {
            if (Material == nullptr)
            {
                return false;
            }

            if (Material->IsReadyForRender() == false)
            {
                return false;
            }
        }

        return !Materials.empty();
    }
}
