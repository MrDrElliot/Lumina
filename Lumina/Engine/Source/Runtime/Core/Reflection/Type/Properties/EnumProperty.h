#pragma once
#include "Core/Object/Class.h"
#include "Core/Object/ObjectPtr.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FEnumProperty : public FProperty
    {
    public:

        FEnumProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
            auto* EnumParams = (const FEnumPropertyParams*) Params;
            CEnum* InternalEnum = EnumParams->EnumFunc();
            Assert(InternalEnum)
            SetEnum(InternalEnum);
        }

        ~FEnumProperty() override;

        void AddProperty(FProperty* Property) override { InnerProperty = static_cast<FNumericProperty*>(Property); }
        LUMINA_API FNumericProperty* GetInnerProperty() const { return InnerProperty; }
        
        void SetEnum(CEnum* InEnum);

        /** Returns the pointer to the internal enum */
        FORCEINLINE CEnum* GetEnum() const { return Enum; }

        
        void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;
        
        void DrawProperty(void* Object) override;
        
        
    private:

        /** Numeric property which represents the current value of this enum */
        FNumericProperty* InnerProperty = nullptr;

        /** The actual enum class object this property represents */
        CEnum* Enum = nullptr;
    
    };
}
