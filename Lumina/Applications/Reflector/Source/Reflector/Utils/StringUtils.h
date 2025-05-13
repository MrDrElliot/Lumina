#pragma once

#include <cstdint>
#include <cstring>

namespace StringUtils
{
    template<typename StringType>
    inline StringType ReplaceAllOccurrences( StringType const& originalString, char const* pSearchString, char const* pReplacement )
    {
        int32_t const searchLength = (int32_t) strlen( pSearchString );
        if ( originalString.empty() || searchLength == 0 )
        {
            return originalString;
        }

        int32_t const replacementLength = ( pReplacement == nullptr ) ? 1 : (int32_t) strlen( pReplacement ) + 1;

        StringType copiedString = originalString;
        auto idx = originalString.find( pSearchString );
        while ( idx != StringType::npos )
        {
            copiedString.replace( idx, searchLength, pReplacement == nullptr ? "" : pReplacement );
            idx = copiedString.find( pSearchString, idx + replacementLength );
        }

        return copiedString;
    }
}
