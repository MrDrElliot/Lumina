#pragma once
#include <clang-c/Index.h>
#include "EASTL/string.h"


namespace Lumina
{
    struct FFieldInfo
    {
        CXType                              Type;
        EPropertyTypeFlags                  Flags;
        eastl::string                       Name;
        eastl::string                       TypeName;
        eastl::vector<eastl::string>        PropertyFlags;
    };
}
