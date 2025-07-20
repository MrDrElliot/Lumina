#include "Mesh.h"

#include "Renderer/RenderContext.h"
#include "Assets/AssetTypes/Material/Material.h"
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

    CMaterial* CMesh::GetMaterialAtSlot(SIZE_T Slot) const
    {
        return Materials.empty() ? nullptr : Cast<CMaterial>(Materials[Slot]);
    }

    bool CMesh::IsReadyForRender() const
    {
        if (HasAnyFlag(OF_NeedsLoad) || HasAllFlags(OF_MarkedGarbage))
        {
            return false;
        }

        for (CMaterial* Material : Materials)
        {
            if (Material == nullptr)
            {
                return false;
            }
            
            if (Material->BindingLayout == nullptr
                || Material->BindingSet == nullptr
                || Material->VertexShader == nullptr
                || Material->PixelShader == nullptr)
            {
                return false;
            }
        }

        return !Materials.empty();
    }
}
