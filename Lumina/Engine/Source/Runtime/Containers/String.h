#pragma once


#include "Core/DisableAllWarnings.h"

PRAGMA_DISABLE_ALL_WARNINGS
#include <spdlog/fmt/fmt.h>
#include "EASTL/fixed_string.h"
#include "EASTL/string.h"
#include <ostream>
PRAGMA_ENABLE_ALL_WARNINGS

#include "Platform/WindowsPlatform.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    using FString = eastl::basic_string<char>;
    using FStringView = eastl::string_view;
    template<eastl_size_t S> using TInlineString = eastl::fixed_string<char, S, true>;
    using FInlineString = eastl::fixed_string<char, 255, true>;
    using FWString = eastl::basic_string<wchar_t>;
    
    
    namespace StringUtils
    {
        // Checks if a string starts with a specific substring.
        template<typename StringType>
        inline bool StartsWith(StringType const& inStr, char const* pStringToMatch)
        {
            size_t const matchStrLen = strlen(pStringToMatch);

            if (inStr.length() < matchStrLen)
            {
                return false;
            }

            // Compare substr
            return strncmp(inStr.c_str(), pStringToMatch, matchStrLen) == 0;
        }
        
        template<typename StringType>
        inline bool EndsWith( StringType const& inStr, char const* pStringToMatch )
        {
            size_t const matchStrLen = strlen( pStringToMatch );

            if ( inStr.length() < matchStrLen )
            {
                return false;
            }

            // Compare substr
            char const* const pSubString = &inStr[inStr.length() - matchStrLen];
            return strcmp( pSubString, pStringToMatch ) == 0;
        }

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

        template<typename StringType>
        inline StringType& ReplaceAllOccurrencesInPlace( StringType& originalString, char const* pSearchString, char const* pReplacement )
        {
            int32_t const searchLength = (int32_t) strlen( pSearchString );
            if ( originalString.empty() || searchLength == 0 )
            {
                return originalString;
            }

            int32_t const replacementLength = ( pReplacement == nullptr ) ? 1 : (int32_t) strlen( pReplacement ) + 1;

            auto idx = originalString.find( pSearchString );
            while ( idx != StringType::npos )
            {
                originalString.replace( idx, searchLength, pReplacement == nullptr ? "" : pReplacement );
                idx = originalString.find( pSearchString, idx + replacementLength );
            }

            return originalString;
        }

        template<typename StringType>
        inline StringType RemoveAllOccurrences( StringType const& originalString, char const* searchString )
        {
            return ReplaceAllOccurrences( originalString, searchString, "" );
        }

        template<typename StringType>
        inline StringType& RemoveAllOccurrencesInPlace( StringType& originalString, char const* searchString )
        {
            return ReplaceAllOccurrencesInPlace( originalString, searchString, "" );
        }

        template<typename StringType>
        inline StringType StripAllWhitespace( StringType const& originalString )
        {
            StringType strippedString = originalString;
            strippedString.erase( eastl::remove( strippedString.begin(), strippedString.end(), ' ' ), strippedString.end() );
            return strippedString;
        }

        inline void StripTrailingWhitespace( char* string )
        {
            size_t const origStringLength = strlen( string );
            FInlineString tmp = string;
            tmp.rtrim();
            strncpy_s( string, origStringLength + 1, tmp.c_str(), tmp.length() );
        }

        template<typename StringType, typename StringTypeVector>
        inline void Split( StringType const& str, StringTypeVector& results, char const* pDelimiter = " ", bool ignoreEmptyStrings = true )
        {
            size_t idx, lastIdx = 0;
            results.clear();

            while ( true )
            {
                idx = str.find_first_of( pDelimiter, lastIdx );
                if ( idx == StringType::npos )
                {
                    idx = str.length();

                    if ( idx != lastIdx || !ignoreEmptyStrings )
                    {
                        results.push_back( StringType( str.data() + lastIdx, idx - lastIdx ) );
                    }
                    break;
                }
                else
                {
                    if ( idx != lastIdx || !ignoreEmptyStrings )
                    {
                        results.push_back( StringType( str.data() + lastIdx, idx - lastIdx ) );
                    }
                }

                lastIdx = idx + 1;
            }
        }

        // Trim leading and trailing whitespace from a string
        template<typename StringType>
        inline StringType Trim(StringType const& originalString)
        {
            StringType trimmedString = originalString;
            // Trim leading whitespace
            trimmedString.erase(trimmedString.begin(), eastl::find_if_not(trimmedString.begin(), trimmedString.end(), ::isspace));
            // Trim trailing whitespace
            trimmedString.erase(eastl::find_if_not(trimmedString.rbegin(), trimmedString.rend(), ::isspace).base(), trimmedString.end());
            return trimmedString;
        }
        
        // Converts string to lower case
        template<typename StringType>
        inline StringType ToLower(StringType const& originalString)
        {
            StringType lowerString = originalString;
            eastl::transform(lowerString.begin(), lowerString.end(), lowerString.begin(), ::tolower);
            return lowerString;
        }

        // Converts string to upper case
        template<typename StringType>
        inline StringType ToUpper(StringType const& originalString)
        {
            StringType upperString = originalString;
            eastl::transform(upperString.begin(), upperString.end(), upperString.begin(), ::toupper);
            return upperString;
        }

        // Joins a list of strings with a delimiter
        template<typename StringType, typename StringTypeVector>
        inline StringType Join(const StringTypeVector& strings, const StringType& delimiter)
        {
            StringType result;
            for (size_t i = 0; i < strings.size(); ++i)
            {
                if (i > 0)
                    result += delimiter;
                result += strings[i];
            }
            return result;
        }

        // Check if a string contains a substring
        template<typename StringType>
        inline bool Contains(const StringType& str, const StringType& substring)
        {
            return str.find(substring) != StringType::npos;
        }

        //-------------------------------------------------------------------------

        // Is this a valid hex character (0-9 & A-F)
        inline bool IsValidHexChar( char ch )
        {
            return (bool) isxdigit( ch );
        }

        int32 CompareInsensitive( char const* pStr0, char const* pStr1 );
        int32 CompareInsensitive( char const* pStr0, char const* pStr1, size_t n );

        //-------------------------------------------------------------------------

        inline int32 CompareInsensitive(char const* pStr0, char const* pStr1)
        {
            return _stricmp( pStr0, pStr1 );
        }

        inline int32 CompareInsensitive(char const* pStr0, char const* pStr1, size_t n)
        {
            return _strnicmp( pStr0, pStr1, n );
        }

        // Convert hex character (0-9 & A-F) to byte value
        inline uint8_t HexCharToByteValue( char ch )
        {
            // 0-9
            if ( ch > 47 && ch < 58 ) return (uint8_t) ( ch - 48 );

            // a-f
            if ( ch > 96 && ch < 103 ) return (uint8_t) ( ch - 87 );

            // A-F
            if ( ch > 64 && ch < 71 ) return (uint8_t) ( ch - 55 );

            return 0;
        }

        //-------------------------------------------------------------------------

        // Convert hex character pair (0-9 & A-F) to byte value
        inline uint8_t HexCharToByteValue( char a, char b )
        {
            return (uint8_t) ( HexCharToByteValue( a ) * 16 + HexCharToByteValue( b ) );
        }

        //-------------------------------------------------------------------------

        FORCEINLINE FWString ToWideString(const FString& str) { return FWString( FWString::CtorConvert(), str ); }
        FORCEINLINE FWString ToWideString(const char* pStr) { return FWString( FWString::CtorConvert(), pStr ); }
        FORCEINLINE FString FromWideString(const FWString& Str) { return FString(FString::CtorConvert(), Str); }
    }
}

#define WIDE_TO_UTF8(Str) \
    StringUtils::FromWideString(Str)

#define UTF8_TO_WIDE(Str) \
    StringUtils::ToWideString(Str)

namespace fmt
{
    template <>
    struct formatter<Lumina::FString>
    {
        constexpr auto parse(::fmt::format_parse_context& ctx) -> decltype(ctx.begin())
        {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const Lumina::FString& str, FormatContext& ctx) -> decltype(ctx.out())
        {
            return fmt::format_to(ctx.out(), "{}", str.data());
        }
    };
}


namespace eastl
{
    template <eastl_size_t S>
    struct hash<eastl::fixed_string<char, S, true>>
    {
        size_t operator()(const eastl::fixed_string<char, S, true>& str) const noexcept
        {
            return eastl::hash<eastl::string_view>{}(eastl::string_view(str.c_str(), str.length()));
        }
    };
}

namespace eastl
{
    inline std::ostream& operator<<(std::ostream& os, const Lumina::FString& str)
    {
        os.write(str.c_str(), str.size());
        return os;
    }
}