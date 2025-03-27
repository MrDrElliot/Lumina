#pragma once
#include "Containers/String.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct FField;

    class Type
    {
    public:
        
        FString*       Name;           // Name of the class.
        FField*        Fields;         // Array of fields in the class
        uint32         NumFields;      // Number of fields
        Type*          Parent;         // Parent type (if any)
    };
}
