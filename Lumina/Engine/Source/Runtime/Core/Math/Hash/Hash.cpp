#include "Hash.h"

#include "xxhash/xxhash.h"

namespace Lumina::Hash
{
    uint32 XXHash::GetHash32(const void* Data, SIZE_T size)
    {
        return XXH32(Data, size, 0);
    }

    uint64 XXHash::GetHash64(const void* Data, SIZE_T size)
    {
        return XXH64(Data, size, 0);
    }
}
