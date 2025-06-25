#pragma once

#include <type_traits>
#include "Core/Serialization/Archiver.h"
#include "Core/Assertions/Assert.h"
#include "Platform/WindowsPlatform.h"

//-------------------------------------------------------------------------
//  Bit Flags
//-------------------------------------------------------------------------
// Generic flag flags type

namespace Lumina
{
    class FBitFlags
    {

    public:

        constexpr static uint8_t const MaxFlags = 32;
        FORCEINLINE static uint32_t GetFlagMask( uint8_t flag ) { return (uint32_t) ( 1u << flag ); }

    public:

        inline FBitFlags() = default;
        inline explicit FBitFlags( uint32_t flags ) : m_flags( flags ) {}

        //-------------------------------------------------------------------------

        FORCEINLINE uint32_t Get() const { return m_flags; }
        FORCEINLINE void Set( uint32_t flags ) { m_flags = flags; }
        inline operator uint32_t() const { return m_flags; }

        FORCEINLINE bool HasNoFlagsSet() const { return m_flags == 0; }
        FORCEINLINE bool IsAnyFlagSet() const { return m_flags != 0; }

        //-------------------------------------------------------------------------

        FORCEINLINE bool IsFlagSet( uint8_t flag ) const
        {
            Assert( flag < MaxFlags );
            return ( m_flags & GetFlagMask( flag ) ) > 0;
        }

        template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
        FORCEINLINE bool IsFlagSet( T enumValue ) const
        {
            return IsFlagSet( (uint8_t) enumValue );
        }

        FORCEINLINE void SetFlag( uint8_t flag )
        {
            Assert( flag >= 0 && flag < MaxFlags );
            m_flags |= GetFlagMask( flag );
        }

        template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
        FORCEINLINE void SetFlag( T enumValue )
        {
            SetFlag( (uint8_t)enumValue );
        }

        FORCEINLINE void SetFlag( uint8_t flag, bool value )
        {
            Assert( flag < MaxFlags );
            value ? SetFlag( flag ) : ClearFlag( flag );
        }

        template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
        FORCEINLINE void SetFlag( T enumValue, bool value )
        {
            SetFlag( (uint8_t)enumValue, value );
        }

        FORCEINLINE void SetAllFlags()
        {
            m_flags = 0xFFFFFFFF;
        }

        //-------------------------------------------------------------------------

        FORCEINLINE bool IsFlagCleared( uint8_t flag ) const
        {
            Assert( flag < MaxFlags );
            return ( m_flags & GetFlagMask( flag ) ) == 0;
        }

        template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
        FORCEINLINE bool IsFlagCleared( T enumValue )
        {
            return IsFlagCleared( (uint8_t)enumValue );
        }

        FORCEINLINE void ClearFlag( uint8_t flag )
        {
            Assert( flag < MaxFlags );
            m_flags &= ~GetFlagMask( flag );
        }

        template<typename T>
        FORCEINLINE void ClearFlag( T enumValue )
        {
            ClearFlag( (uint8_t)enumValue );
        }

        FORCEINLINE void ClearAllFlags()
        {
            m_flags = 0;
        }

        //-------------------------------------------------------------------------

        FORCEINLINE void FlipFlag( uint8_t flag )
        {
            Assert( flag >= 0 && flag < MaxFlags );
            m_flags ^= GetFlagMask( flag );
        }

        template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
        FORCEINLINE void FlipFlag( T enumValue )
        {
            FlipFlag( (uint8_t)enumValue );
        }


        FORCEINLINE void FlipAllFlags()
        {
            m_flags = ~m_flags;
        }

        //-------------------------------------------------------------------------

        FORCEINLINE FBitFlags& operator| ( uint8_t flag )
        {
            Assert( flag < MaxFlags );
            m_flags |= GetFlagMask( flag );
            return *this;
        }

        FORCEINLINE FBitFlags& operator& ( uint8_t flag )
        {
            Assert( flag < MaxFlags );
            m_flags &= GetFlagMask( flag );
            return *this;
        }

        friend FArchive& operator << (FArchive& Ar, FBitFlags& Data)
        {
            Ar << Data.m_flags;
            return Ar;
        }


    protected:

        uint32 m_flags = 0;
    };
}

//-------------------------------------------------------------------------
//  Templatized Bit Flags
//-------------------------------------------------------------------------
// Helper to create flag flags variables from a specific enum type

namespace Lumina
{
    template<typename T>
    class TBitFlags : public FBitFlags
    {
        static_assert( std::is_enum<T>::value, "TBitFlags only supports enum types" );
        static_assert( sizeof( T ) <= sizeof( uint32_t ), "enum type is too large to be used as bitflags" );

    public:

        using FBitFlags::FBitFlags;

        inline explicit TBitFlags( T value ) 
            : FBitFlags( GetFlagMask( (uint8_t) value ) )
        {
            Assert( (uint32_t) value < MaxFlags );
        }

        inline TBitFlags( uint32_t i )
            : FBitFlags( i )
        {}

        inline TBitFlags( TBitFlags<T> const& flags )
            : FBitFlags( flags.m_flags )
        {}

        template<typename... Args, class Enable = std::enable_if_t<( ... && std::is_convertible_v<Args, T> )>>
        TBitFlags( Args&&... args )
        {
            ( ( m_flags |= 1u << (uint8_t) args ), ... );
        }

        TBitFlags& operator=( TBitFlags const& rhs ) = default;

        //-------------------------------------------------------------------------

        FORCEINLINE bool IsFlagSet( T flag ) const { return FBitFlags::IsFlagSet( (uint8_t) flag ); }
        FORCEINLINE bool IsFlagCleared( T flag ) const { return FBitFlags::IsFlagCleared( (uint8_t) flag ); }
        FORCEINLINE void SetFlag( T flag ) { FBitFlags::SetFlag( (uint8_t) flag ); }
        FORCEINLINE void SetFlag( T flag, bool value ) { FBitFlags::SetFlag( (uint8_t) flag, value ); }
        FORCEINLINE void FlipFlag( T flag ) { FBitFlags::FlipFlag( (uint8_t) flag ); }
        FORCEINLINE void ClearFlag( T flag ) { FBitFlags::ClearFlag( (uint8_t) flag ); }

        //-------------------------------------------------------------------------

        template<typename... Args>
        inline void SetMultipleFlags( Args&&... args )
        {
            ( ( m_flags |= 1u << (uint8_t) eastl::forward<Args>(args)), ... );
        }

        template<typename... Args>
        inline bool AreAnyFlagsSet( Args&&... args ) const
        {
            uint32_t mask = 0;
            ( ( mask |= 1u << (uint8_t) eastl::forward<Args>(args)), ... );
            return ( m_flags & mask ) != 0;
        }

        //-------------------------------------------------------------------------

        FORCEINLINE TBitFlags& operator| ( T flag )
        {
            Assert( (uint8_t) flag < MaxFlags );
            m_flags |= GetFlagMask( flag );
            return *this;
        }

        FORCEINLINE TBitFlags& operator& ( T flag )
        {
            Assert( (uint8_t) flag < MaxFlags );
            m_flags &= GetFlagMask( flag );
            return *this;
        }
        
    };

    //-------------------------------------------------------------------------

    enum class BitFlagsValidation : uint32_t {};
    static_assert( sizeof( TBitFlags<BitFlagsValidation> ) == sizeof( FBitFlags ), "TBitFlags is purely syntactic sugar for easy conversion of enums to flags. It must not contain any members!" );
}
