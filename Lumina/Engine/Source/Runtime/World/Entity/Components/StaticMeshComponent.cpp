#include "StaticMeshComponent.h"
#include "Renderer/RHIIncl.h"


namespace Lumina
{
    CMaterialInterface* SStaticMeshComponent::GetMaterialForSlot(SIZE_T Slot)
    {
        if (!StaticMesh.IsValid())
        {
            return nullptr;
        }
        
        if (Slot < MaterialOverrides.size() && MaterialOverrides[Slot])
        {
            return MaterialOverrides[Slot];
        }
        else
        {
            return StaticMesh->GetMaterialAtSlot(Slot);
        }
    }

}