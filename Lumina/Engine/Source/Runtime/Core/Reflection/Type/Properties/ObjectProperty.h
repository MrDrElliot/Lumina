#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FObjectProperty : public FProperty
    {
    public:
        DECLARE_FPROPERTY(EPropertyTypeFlags::Object)

        FObjectProperty(const FFieldOwner& InOwner, const FObjectPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
            ObjectClass = Params->ClassFunc();
            ElementSize = sizeof(void*);
        }

        void Serialize(FArchive& Ar, void* Value) override;
        void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;

        LUMINA_API CClass* GetPropertyClass() const { return ObjectClass; }
        
    private:
        
        CClass* ObjectClass = nullptr;
    };
}
