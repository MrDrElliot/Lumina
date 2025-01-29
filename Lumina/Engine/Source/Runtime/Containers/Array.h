#pragma once

#include "Core/Serialization/Archiver.h"
#include "Core/Templates/CanBulkSerialize.h"
#include "Platform/GenericPlatform.h"
#include "EASTL/vector.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/array.h"

//-------------------------------------------------------------------------
#define InvalidIndex -1

namespace Lumina
{
    //-------------------------------------------------------------------------
    // Most commonly used containers aliases
    //-------------------------------------------------------------------------

    template<typename T> using TVector = eastl::vector<T>;
    template<typename T, eastl_size_t S> using TInlineVector = eastl::fixed_vector<T, S, true>;
    template<typename T, eastl_size_t S> using TArray = eastl::array<T, S>;

    using Blob = TVector<uint8>;


    //-------------------------------------------------------------------------
    // Serialization of TVector, TInlineVector, and TArray
    //-------------------------------------------------------------------------

    template<typename ValueType>
    FArchive& operator << (FArchive& Ar, TVector<ValueType>& Array)
    {
        eastl_size_t SerializeNum = 0;

        if (Ar.IsReading())
        {
            Ar << SerializeNum; // Read the size
            Array.clear(); // Ensure the array is empty before resizing
        }
        else
        {
            SerializeNum = Array.size(); // Use current size when writing
        }

        if (Ar.HasError())
        {
            return Ar;
        }

        // Case for bulk serialization (e.g., small types like uint8 or types that support it)
        if constexpr (sizeof(ValueType) == 1 || TCanBulkSerialize<ValueType>::Value)
        {
            if (Ar.IsReading())
            {
                Array.resize(SerializeNum); // Resize the array based on serialized number
            }

            Ar.Serialize(Array.data(), SerializeNum * sizeof(ValueType)); // Bulk serialize data
        }
        else
        {
            if (Ar.IsReading())
            {
                for (eastl_size_t i = 0; i < SerializeNum; i++)
                {
                    Ar << Array.emplace_back();
                }
            }
            else
            {
                for (eastl_size_t i = 0; i < SerializeNum; i++)
                {
                    Ar << Array[i];
                }
            }
        }

        return Ar;
    }

    
    //-------------------------------------------------------------------------
    // Simple utility functions to improve syntactic usage of container types
    //-------------------------------------------------------------------------

    // Find an element in a vector
    template<typename T>
    inline typename TVector<T>::const_iterator VectorFind( TVector<T> const& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Find an element in a vector
    // Usage: vectorFind( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline typename TVector<T>::const_iterator VectorFind( TVector<T> const& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    // Find an element in a vector
    // Require non-const versions since we might want to modify the result
    template<typename T>
    inline typename TVector<T>::iterator VectorFind( TVector<T>& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Find an element in a vector
    // Usage: vectorFind( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    // Require non-const versions since we might want to modify the result
    template<typename T, typename V, typename Predicate>
    inline typename TVector<T>::iterator VectorFind( TVector<T>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    template<typename T, typename V>
    inline bool VectorContains( TVector<T> const& vector, V const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    // Usage: VectorContains( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline bool VectorContains( TVector<T> const& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) ) != vector.end();
    }

    // Usage: VectorContains( vector, [] ( T const& typeRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline bool VectorContains( TVector<T> const& vector, Predicate predicate )
    {
        return eastl::find_if( vector.begin(), vector.end(), eastl::forward<Predicate>( predicate ) ) != vector.end();
    }

    template<typename T>
    inline int32_t VectorFindIndex( TVector<T> const& vector, T const& value )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return (int32_t) ( iter - vector.begin() );
        }
    }

    // Usage: VectorFindIndex( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline int32_t VectorFindIndex( TVector<T> const& vector, V const& value, Predicate predicate )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value, predicate );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return (int32_t) ( iter - vector.begin() );
        }
    }

    // Usage: VectorContains( vector, [] ( T const& typeRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline int32_t VectorFindIndex( TVector<T> const& vector, Predicate predicate )
    {
        auto iter = eastl::find_if( vector.begin(), vector.end(), predicate );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return (int32_t) ( iter - vector.begin() );
        }
    }

    //-------------------------------------------------------------------------

    template<typename T, typename V, eastl_size_t S>
    inline bool VectorContains( TInlineVector<T, S> const& vector, V const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    template<typename T, eastl_size_t S, typename V, typename Predicate>
    inline bool VectorContains( TInlineVector<T, S> const& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) ) != vector.end();
    }

    // Find an element in a vector
    template<typename T, typename V, eastl_size_t S>
    inline typename TInlineVector<T, S>::const_iterator VectorFind( TInlineVector<T, S> const& vector, V const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Find an element in a vector
    template<typename T, typename V, eastl_size_t S, typename Predicate>
    inline typename TInlineVector<T, S>::const_iterator VectorFind( TInlineVector<T, S> const& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    // Find an element in a vector
    // Require non-const versions since we might want to modify the result
    template<typename T, typename V, eastl_size_t S>
    inline typename TInlineVector<T, S>::iterator VectorFind( TInlineVector<T, S>& vector, V const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Find an element in a vector
    // Require non-const versions since we might want to modify the result
    template<typename T, typename V, eastl_size_t S, typename Predicate>
    inline typename TInlineVector<T, S>::iterator VectorFind( TInlineVector<T, S>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    template<typename T, typename V, eastl_size_t S>
    inline int32_t VectorFindIndex( TInlineVector<T, S> const& vector, V const& value )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return ( int32_t) ( iter - vector.begin() );
        }
    }

    template<typename T, typename V, eastl_size_t S, typename Predicate>
    inline int32_t VectorFindIndex( TInlineVector<T, S> const& vector, V const& value, Predicate predicate )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value, predicate );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return ( int32_t) ( iter - vector.begin() );
        }
    }

    //-------------------------------------------------------------------------

    template<typename T>
    inline void VectorEmplaceBackUnique( TVector<T>& vector, T&& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }

    template<typename T>
    inline void VectorEmplaceBackUnique( TVector<T>& vector, T const& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }

    template<typename T, eastl_size_t S>
    inline void VectorEmplaceBackUnique( TInlineVector<T,S>& vector, T&& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }

    template<typename T, eastl_size_t S>
    inline void VectorEmplaceBackUnique( TInlineVector<T, S>& vector, T const& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }
}