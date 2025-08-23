#include "StaticMeshComponent.h"
#include "Renderer/RHIIncl.h"


namespace Lumina
{
    CMaterialInterface* SStaticMeshComponent::GetMaterialForSlot(SIZE_T Slot)
    {
        LUMINA_PROFILE_SCOPE();
        
        if (Slot < MaterialOverrides.size() && MaterialOverrides[Slot])
        {
            return MaterialOverrides[Slot];
        }
        else if (StaticMesh.IsValid())
        {
            return StaticMesh->GetMaterialAtSlot(Slot);
        }

        return nullptr;
    }
}