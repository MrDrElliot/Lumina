#include "ReflectedProperty.h"

namespace Lumina
{

    void FReflectedProperty::AppendPropertyDef(std::stringstream& SS, const char* PropertyFlags, const char* TypeFlags) const
    {
        SS << "{ " << "\"" <<  Name.c_str() << "\"" << ", " << PropertyFlags << ", " << TypeFlags << ", offsetof(" << Outer.c_str() << ", " << Name.c_str() << "), };\n";
    }
}
