#include "ArrayProperty.h"

namespace Lumina
{
    void FArrayProperty::Serialize(FArchive& Ar, void* Value)
    {
        FReflectArrayHelper Helper(this, Value);
        SIZE_T ElementCount = Helper.Num();
        
        if (Ar.IsWriting())
        {
            Ar << ElementCount;
            for (SIZE_T i = 0; i < ElementCount; i++)
            {
                Inner->Serialize(Ar, Helper.GetRawAt(i));
            }
        }
        else
        {
            Ar << ElementCount;
            Helper.Resize(ElementCount);

            for (SIZE_T i = 0; i < ElementCount; ++i)
            {
                Inner->Serialize(Ar, Helper.GetRawAt(i));
            }
        }
        
    }

    void FArrayProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        FArchive* InnerAr = Slot.GetStructuredArchive()->GetInnerAr();
        FReflectArrayHelper Helper(this, Value);
        SIZE_T ElementCount = Helper.Num();
        
        if (InnerAr->IsWriting())
        {
            Slot.Serialize(ElementCount);
            for (SIZE_T i = 0; i < ElementCount; i++)
            {
                Inner->SerializeItem(Slot, Helper.GetRawAt(i));
            }
        }
        else
        {
            Slot.Serialize(ElementCount);
            Helper.Resize(ElementCount);

            for (SIZE_T i = 0; i < ElementCount; ++i)
            {
                Inner->SerializeItem(Slot, Helper.GetRawAt(i));
            }
        }
        
    }

}
