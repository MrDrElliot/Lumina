#pragma once
#include "Core/LuminaMacros.h"


namespace Lumina
{
    /** Core object flags for all CObjects, describes it's state. */
    enum EObjectFlags
    {
        /** NULL */
        OF_None                 = 0,

        /** Should not be saved */
        OF_Transient            = 1 << 0,
        
        /** Is this a default subobject of a class? */
        OF_DefaultObject        = 1 << 1,

        /** Is this object already marked for garbage? */
        OF_MarkedGarbage        = 1 << 2,

        /** Does this object need to be loaded after creation? */
        OF_NeedsLoad            = 1 << 3,

        /** Was this object loaded from a package */
        OF_WasLoaded            = 1 << 4,

        /** Object is referenced outside of it's package */
        OF_Public              = 1 << 5,
    };

    ENUM_CLASS_FLAGS(EObjectFlags);

    LUMINA_API inline FInlineString ObjectFlagsToString(EObjectFlags Flags)
    {
        FInlineString Result;

        if (Flags == OF_None)
        {
            return "None";
        }

        if (EnumHasAnyFlags(Flags, OF_Transient))       Result += "OF_Transient|";
        if (EnumHasAnyFlags(Flags, OF_DefaultObject))   Result += "OF_DefaultObject|";
        if (EnumHasAnyFlags(Flags, OF_MarkedGarbage))   Result += "OF_MarkedGarbage|";
        if (EnumHasAnyFlags(Flags, OF_NeedsLoad))       Result += "OF_NeedsLoad|";
        if (EnumHasAnyFlags(Flags, OF_WasLoaded))       Result += "OF_WasLoaded|";
        if (EnumHasAnyFlags(Flags, OF_Public))          Result += "OF_Public|";

        if (!Result.empty() && Result.back() == '|') Result.pop_back();

        return Result;
    }
}
