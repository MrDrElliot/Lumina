#include "StructProperty.h"

namespace Lumina
{
    void FStructProperty::Serialize(FArchive& Ar, void* Value)
    {
        Struct->SerializeTaggedProperties(Ar, Value);
    }
}
