#pragma once
#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Core/Variant/Variant.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FField;

    enum class EFieldFlags : uint32
    {
        None      = 0,
        Editable  = 1 << 0,
        Nullable  = 1 << 1,  // Example: allow null values for the field
        Default   = 1 << 2,  // Example: a default value is provided for the field
    };

    ENUM_CLASS_FLAGS(EFieldFlags)

    struct FFieldOwner
    {
        TVariant<CObject*, FField*>     Variant;

    };
    
    class FField
    {
    public:

        FField(FFieldOwner InOwner)
            :Owner(InOwner)
        {
            Offset = 0;
            Next = nullptr;
        }

        // Mirrored in CClass.
        LUMINA_API virtual void AddProperty(FProperty* Property) { std::unreachable(); };

        
        FString         Name;
        uint32          Offset;
        FField*         Next;
        FFieldOwner     Owner;
    };
    
}
