#pragma once

#include "ObjectFlags.h"
#include "Containers/Name.h"


namespace Lumina
{
    class CClass;

    struct FConstructCObjectParams
    {
        FConstructCObjectParams(const CClass* InClass)
            : Class(InClass)
            , Name()
            , Package()
            , Flags(EObjectFlags::OF_None)
        {}
        
        const CClass* Class;
        FName Name;
        FName Package;
        EObjectFlags Flags;
    };
}
