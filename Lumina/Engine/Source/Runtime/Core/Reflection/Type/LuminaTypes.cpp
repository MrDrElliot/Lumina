#include "LuminaTypes.h"
#include "Core/Object/Field.h"
#include "Core/Object/Class.h"

namespace Lumina
{
    void FProperty::Init()
    {
        eastl::visit([this]<typename T0>(T0& Value)
        {
            if constexpr (std::is_pointer_v<std::decay_t<T0>>)
            {
                Value->AddProperty(this);
            }
        }, Owner.Variant);
   
    }

    FString FProperty::GetTypeAsString() const
    {
        return PropertyTypeToString(TypeFlags);
    }

    void* FProperty::GetValuePtrInternal(void* ContainerPtr, int32 ArrayIndex) const
    {
        void* PropertyPtr = (uint8*)ContainerPtr + Offset;
        return (uint8*)PropertyPtr + ArrayIndex * (size_t)ElementSize;
    }
    
}
