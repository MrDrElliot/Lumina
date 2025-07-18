#include "Mesh.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"


namespace Lumina
{
    void CMesh::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
        Ar << MeshResource;
    }

    void CMesh::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CMesh::PostLoad()
    {
        
    }

    bool CMesh::IsReadyForRender() const
    {
        if (HasAnyFlag(OF_NeedsLoad) || HasAllFlags(OF_MarkedGarbage))
        {
            return false;
        }

        //@ TODO Temp until reflection is better supported.
        CMaterial* CastedMaterial = Cast<CMaterial>(Material);
        
        if (CastedMaterial == nullptr)
        {
            return false;
        }

        if (CastedMaterial->BindingLayout == nullptr
            || CastedMaterial->BindingSet == nullptr
            || CastedMaterial->VertexShader == nullptr
            || CastedMaterial->PixelShader == nullptr)
        {
            return false;
        }

        return true;
    }
}
