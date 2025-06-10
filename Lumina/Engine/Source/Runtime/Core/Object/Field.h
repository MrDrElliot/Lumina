#pragma once
#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Core/Assertions/Assert.h"
#include "Core/Variant/Variant.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FField;
    class CStruct;
    class FProperty;

    enum class EFieldFlags : uint32
    {
        None      = 0,
        Editable  = 1 << 0,
        Nullable  = 1 << 1,
        Default   = 1 << 2,
    };

    ENUM_CLASS_FLAGS(EFieldFlags)

    struct FFieldOwner
    {
        TVariant<CStruct*, FField*>     Variant;
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

        virtual ~FField() = default;

        // Mirrored in CStruct.
        LUMINA_API virtual void AddProperty(FProperty* Property) { LUMINA_NO_ENTRY(); }

        
        FName           Name;
        uint32          Offset;
        FField*         Next;
        FFieldOwner     Owner;
    };
    
}
