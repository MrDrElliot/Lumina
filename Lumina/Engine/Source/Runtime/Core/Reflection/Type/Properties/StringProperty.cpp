#include "StringProperty.h"

namespace Lumina
{
    void FStringProperty::Serialize(FArchive& Ar, void* Value)
    {
        FString* StringValue = (FString*)Value;
        Ar << *StringValue;
    }

    void FNameProperty::Serialize(FArchive& Ar, void* Value)
    {
        FName* StringValue = (FName*)Value;
        Ar << *StringValue;
    }
}
