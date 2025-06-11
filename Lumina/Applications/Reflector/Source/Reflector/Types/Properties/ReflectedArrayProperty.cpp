#include "ReflectedArrayProperty.h"

namespace Lumina
{
    void FReflectedArrayProperty::AppendDefinition(eastl::string& Stream) const
    {
        Stream += "{ \"" +  Name + "\"" + ", Lumina::EPropertyFlags::None, " + "Lumina::EPropertyTypeFlags::Vector," +  " offsetof(" + Outer + ", " + Name + ") };\n";
    }
}
