#pragma once

#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Module/API.h"

namespace Lumina
{
    class LUMINA_API FPropertyMetadata
    {
    public:

        void AddValue(const FName& Key, const FName& Value);

        bool HasMetadata(const FName& Key);

        FName GetMetadata(const FName& Key);
    

    private:

        THashMap<FName, FName>              PairParams;
    };
}
