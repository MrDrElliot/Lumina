#include "ReflectedArrayProperty.h"

namespace Lumina
{
    void FReflectedArrayProperty::AppendDefinition(eastl::string& Stream) const
    {
        AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::Vector");
    }
}
