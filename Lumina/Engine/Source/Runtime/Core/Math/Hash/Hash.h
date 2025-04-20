#pragma once
#include <cstring>

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"


//-----------------------------------------------------------------------------

namespace Lumina::Hash
{
    // XXHash
    //-------------------------------------------------------------------------
    // This is the default hashing algorithm for the engine

    LUMINA_API FORCEINLINE void HashCombine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    namespace XXHash
    {
        LUMINA_API uint32 GetHash32( void const* pData, size_t size );

        LUMINA_API FORCEINLINE uint32 GetHash32( const FString& string )
        {
            return GetHash32( string.c_str(), string.length() );
        }

        LUMINA_API FORCEINLINE uint32 GetHash32( char const* pString )
        {
            return GetHash32( pString, strlen( pString ) );
        }

        LUMINA_API FORCEINLINE uint32 GetHash32(float Value)
        {
            return GetHash32(&Value, sizeof(float));
        }
        
        LUMINA_API FORCEINLINE uint32 GetHash32( Blob const& data )
        {
            return GetHash32( data.data(), data.size() );
        }

        //-------------------------------------------------------------------------

        LUMINA_API uint64_t GetHash64( void const* pData, size_t size );

        LUMINA_API FORCEINLINE uint64_t GetHash64( const FString& string )
        {
            return GetHash64( string.c_str(), string.length() );
        }

        LUMINA_API FORCEINLINE uint64_t GetHash64( char const* pString )
        {
            return GetHash64( pString, strlen( pString ) );
        }

        LUMINA_API FORCEINLINE uint64_t GetHash64( Blob const& data )
        {
            return GetHash64( data.data(), data.size() );
        }
    }

    // FNV1a
    //-------------------------------------------------------------------------
    // This is a const expression hash
    // Should not be used for anything other than code only features i.e. custom RTTI etc...

    namespace FNV1a
    {
        constexpr uint32 const g_constValue32 = 0x811c9dc5;
        constexpr uint32 const g_defaultOffsetBasis32 = 0x1000193;
        constexpr uint64_t const g_constValue64 = 0xcbf29ce484222325;
        constexpr uint64_t const g_defaultOffsetBasis64 = 0x100000001b3;

        constexpr static inline uint32 GetHash32( char const* const str, const uint32 val = g_constValue32 )
        {
            return ( str[0] == '\0' ) ? val : GetHash32( &str[1], ( (uint64_t) val ^ uint32( str[0] ) ) * g_defaultOffsetBasis32 );
        }

        constexpr static inline uint64_t GetHash64( char const* const str, const uint64_t val = g_constValue64 )
        {
            return ( str[0] == '\0' ) ? val : GetHash64( &str[1], ( (uint64_t) val ^ uint64_t( str[0] ) ) * g_defaultOffsetBasis64 );
        }
    }

    // Default EE hashing functions
    //-------------------------------------------------------------------------

    FORCEINLINE uint32 GetHash32( const FString& string )
    {
        return XXHash::GetHash32( string.c_str(), string.length() );
    }

    template<size_t S>
    FORCEINLINE uint32 GetHash32( TInlineString<S> const& string )
    {
        return XXHash::GetHash32( string.c_str(), string.length() );
    }

    FORCEINLINE uint32 GetHash32( char const* pString )
    {
        return XXHash::GetHash32( pString, strlen( pString ) );
    }

    FORCEINLINE uint32 GetHash32( void const* pPtr, size_t size )
    {
        return XXHash::GetHash32( pPtr, size );
    }

    FORCEINLINE uint32 GetHash32( Blob const& data )
    {
        return XXHash::GetHash32( data.data(), data.size() );
    }

    FORCEINLINE uint64_t GetHash64( const FString& string )
    {
        return XXHash::GetHash64( string.c_str(), string.length() );
    }

    template<size_t S>
    FORCEINLINE uint64_t GetHash64( TInlineString<S> const& string )
    {
        return XXHash::GetHash64( string.c_str(), string.length() );
    }

    FORCEINLINE uint64_t GetHash64( char const* pString )
    {
        return XXHash::GetHash64( pString, strlen( pString ) );
    }

    FORCEINLINE uint64_t GetHash64( void const* pPtr, size_t size )
    {
        return XXHash::GetHash64( pPtr, size );
    }

    FORCEINLINE uint64_t GetHash64( Blob const& data )
    {
        return XXHash::GetHash64( data.data(), data.size() );
    }

    template <typename T>
    requires requires { eastl::hash<T>(); }
    SIZE_T GetHash(const T& value)
    {
        return eastl::hash<T>()(value);
    }
    
    template <class T>
    void HashCombine(SIZE_T& seed, const T& v)
    {
        eastl::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}