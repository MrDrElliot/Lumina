#include "LuminaTypes.h"
#include "Core/Object/Field.h"
#include "Core/Object/Class.h"

namespace Lumina
{
    void FProperty::Init()
    {
        eastl::visit([this](auto& Value)
        {
            if constexpr (std::is_pointer_v<std::decay_t<decltype(Value)>>)
            {
                Value->AddProperty(this);
            }
        }, Owner.Variant);
   
    }

    void* FProperty::GetValuePtrInternal(void* ContainerPtr, int32 ArrayIndex) const
    {
        void* PropertyPtr = (uint8*)ContainerPtr + Offset;
        return (uint8*)PropertyPtr + ArrayIndex * (size_t)ElementSize;
    }

    void FNumericProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        void* PropertyPtr = (uint8*)Value + Offset;
        Slot.Serialize(PropertyPtr, ElementSize);
    }
    
}
