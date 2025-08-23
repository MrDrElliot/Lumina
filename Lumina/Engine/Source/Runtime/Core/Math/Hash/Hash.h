#pragma once

#include <cstring>

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Profiler/Profile.h"
#include "Module/API.h"
#include "Platform/GenericPlatform.h"


//-----------------------------------------------------------------------------

namespace Lumina::Hash
{
    // XXHash
    //-------------------------------------------------------------------------
    // This is the default hashing algorithm for the engine

    LUMINA_API FORCEINLINE void HashCombine(SIZE_T& seed, SIZE_T value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    namespace XXHash
    {
        LUMINA_API uint32 GetHash32(const void* Data, SIZE_T size);

        LUMINA_API FORCEINLINE uint32 GetHash32( const FString& string )
        {
            return GetHash32( string.c_str(), string.length() );
        }

        LUMINA_API FORCEINLINE uint32 GetHash32(const char* String)
        {
            return GetHash32(String, strlen(String));
        }

        LUMINA_API FORCEINLINE uint32 GetHash32(float Value)
        {
            return GetHash32(&Value, sizeof(float));
        }
        
        LUMINA_API FORCEINLINE uint32 GetHash32(const Blob& data)
        {
            return GetHash32(data.data(), data.size());
        }

        //-------------------------------------------------------------------------

        LUMINA_API uint64 GetHash64( void const* pData, SIZE_T size );

        LUMINA_API FORCEINLINE uint64 GetHash64( const FString& string )
        {
            return GetHash64( string.c_str(), string.length() );
        }

        LUMINA_API FORCEINLINE uint64 GetHash64(const char* String)
        {
            return GetHash64(String, strlen(String));
        }

        LUMINA_API FORCEINLINE uint64 GetHash64(const Blob& data)
        {
            return GetHash64(data.data(), data.size());
        }
    }

    // FNV1a
    //-------------------------------------------------------------------------
    // This is a const expression hash
    // Should not be used for anything other than code only features i.e. custom RTTI etc...

    namespace FNV1a
    {
        constexpr uint32 const GConstValue32 = 0x811c9dc5;
        constexpr uint32 const GDefaultOffsetBasis32 = 0x1000193;
        constexpr uint64 const GConstValue64 = 0xcbf29ce484222325;
        constexpr uint64 const GDefaultOffsetBasis64 = 0x100000001b3;

        constexpr static inline uint32 GetHash32(const char* const str, const uint32 val = GConstValue32)
        {
            return ( str[0] == '\0' ) ? val : GetHash32( &str[1], ( (uint64_t) val ^ uint32( str[0] ) ) * GDefaultOffsetBasis32 );
        }

        constexpr static inline uint64_t GetHash64( char const* const str, const uint64_t val = GConstValue64 )
        {
            return ( str[0] == '\0' ) ? val : GetHash64( &str[1], ( (uint64_t) val ^ uint64_t( str[0] ) ) * GDefaultOffsetBasis64 );
        }
    }

    // Default EE hashing functions
    //-------------------------------------------------------------------------

    FORCEINLINE uint32 GetHash32(const FString& string)
    {
        return XXHash::GetHash32(string.c_str(), string.length());
    }

    template<SIZE_T S>
    FORCEINLINE uint32 GetHash32(const TInlineString<S>& string )
    {
        return XXHash::GetHash32(string.c_str(), string.length());
    }

    FORCEINLINE uint32 GetHash32(const char* String)
    {
        return XXHash::GetHash32(String, strlen(String));
    }

    FORCEINLINE uint32 GetHash32(const void* Data, SIZE_T size)
    {
        return XXHash::GetHash32(Data, size);
    }

    FORCEINLINE uint32 GetHash32(const Blob& data)
    {
        return XXHash::GetHash32(data.data(), data.size());
    }

    FORCEINLINE uint64 GetHash64(const FString& string)
    {
        return XXHash::GetHash64(string.c_str(), string.length());
    }

    template<SIZE_T S>
    FORCEINLINE uint64 GetHash64(const TInlineString<S>& string)
    {
        return XXHash::GetHash64(string.c_str(), string.length());
    }

    FORCEINLINE uint64 GetHash64(const char* String)
    {
        return XXHash::GetHash64(String, strlen(String));
    }

    FORCEINLINE uint64 GetHash64(const void* Data, SIZE_T size)
    {
        return XXHash::GetHash64(Data, size);
    }

    FORCEINLINE uint64 GetHash64(const Blob& Data)
    {
        return XXHash::GetHash64(Data.data(), Data.size());
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