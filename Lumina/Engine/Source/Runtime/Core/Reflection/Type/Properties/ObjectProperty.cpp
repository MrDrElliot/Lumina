#include "ObjectProperty.h"

#include "Core/Object/Object.h"
#include "Core/Object/ObjectHandleTyped.h"


namespace Lumina
{
    
    void FObjectProperty::Serialize(FArchive& Ar, void* Value)
    {
        TObjectHandle<CObject>* ObjectHandle = (TObjectHandle<CObject>*)Value;
        FObjectHandle* Ptr = (FObjectHandle*)ObjectHandle;
        Ar << *Ptr;
    }

    void FObjectProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        
    }
    
}
