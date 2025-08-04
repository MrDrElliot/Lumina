#pragma once

#include "Containers/Name.h"
#include "Containers/String.h"
#include "Core/Assertions/Assert.h"
#include "Core/Variant/Variant.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FField;
    class CStruct;
    class FProperty;
    
    struct FFieldOwner
    {
        TVariant<CStruct*, FField*>     Variant;
    };
    
    class FField
    {
    public:

        FField(const FFieldOwner& InOwner)
            :Owner(InOwner)
        {
            Offset = 0;
            Next = nullptr;
        }

        virtual ~FField() = default;

        // Mirrored in CStruct.
        LUMINA_API virtual void AddProperty(FProperty* Property) { LUMINA_NO_ENTRY(); }

        const FName& GetPropertyName() const { return Name; }
        const FString& GetPropertyDisplayName() const { return DisplayName; }
        
        
        FName               Name;
        FString             DisplayName;
        
        uint32              Offset;
        FField*             Next;
        FFieldOwner         Owner;
    };
    
}
