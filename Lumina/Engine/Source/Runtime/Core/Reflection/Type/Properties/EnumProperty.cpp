#include "EnumProperty.h"

namespace Lumina
{
    FEnumProperty::~FEnumProperty()
    {
        FMemory::Delete(InnerProperty);
    }

    void FEnumProperty::SetEnum(CEnum* InEnum)
    {
        Enum = InEnum;
    }

    void FEnumProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        uint64 InternalValue = *InnerProperty->GetValuePtr<uint64>(Value, 0);
        Slot.Serialize(InternalValue);
    }
}
