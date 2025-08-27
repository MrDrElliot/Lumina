#include "StaticMeshComponent.h"

#include "assets/assettypes/material/MaterialInterface.h"
#include "Renderer/RHIIncl.h"


namespace Lumina
{
    CMaterialInterface* SStaticMeshComponent::GetMaterialForSlot(SIZE_T Slot) const
    {
        LUMINA_PROFILE_SCOPE();
        
        if (Slot < MaterialOverrides.size())
        {
            if (CMaterialInterface* Interface = MaterialOverrides[Slot])
            {
                return Interface;
            }
        }
        
        if (StaticMesh.IsValid())
        {
            return StaticMesh->GetMaterialAtSlot(Slot);
        }

        return nullptr;
    }
}