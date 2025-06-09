#pragma once
#include "Core/Object/Class.h"
#include "Core/Object/ObjectPtr.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FEnumProperty : public FProperty
    {
    public:

        FEnumProperty(FFieldOwner InOwner, FPropertyParams* Params = nullptr)
            :FProperty(InOwner, Params)
        {
        }

        ~FEnumProperty() override;

        void AddProperty(FProperty* Property) override { InnerProperty = static_cast<FNumericProperty*>(Property); }
        
        void SetEnum(CEnum* InEnum);

        /** Returns the pointer to the internal enum */
        FORCEINLINE CEnum* GetEnum() const { return Enum; }

        
        void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;
        
    private:

        /** Numeric property which represents the current value of this enum */
        FNumericProperty*   InnerProperty = nullptr;

        /** The actual enum class object this property represents */
        TObjectPtr<CEnum>   Enum;
    
    };
}
