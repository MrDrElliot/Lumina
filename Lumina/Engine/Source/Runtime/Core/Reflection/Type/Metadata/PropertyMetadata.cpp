#include "PropertyMetadata.h"

namespace Lumina
{
    void FPropertyMetadata::AddValue(const FName& Key, const FName& Value)
    {
        PairParams.emplace(Key, Value);
    }

    bool FPropertyMetadata::HasMetadata(const FName& Key)
    {
        return PairParams.find(Key) != PairParams.end();
    }

    FName FPropertyMetadata::GetMetadata(const FName& Key)
    {
        return PairParams.at(Key);
    }
}
