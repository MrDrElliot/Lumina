#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FObjectProperty : public FProperty
    {
    public:

        FObjectProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
            ElementSize = sizeof(void*);    
        }

        void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;

        void DrawProperty(void* Object) override;
        
    };
}
