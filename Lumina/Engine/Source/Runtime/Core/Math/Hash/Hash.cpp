#include "Hash.h"

#include "xxhash/xxhash.h"

namespace Lumina::Hash
{
    uint32_t XXHash::GetHash32( void const* pData, size_t size )
    {
        return XXH32( pData, size, 0 );
    }

    uint64_t XXHash::GetHash64( void const* pData, size_t size )
    {
        return XXH64( pData, size, 0 );
    }
}
