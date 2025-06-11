#pragma once

#include "Core/Serialization/Archiver.h"
#include "Core/Templates/CanBulkSerialize.h"
#include "Platform/GenericPlatform.h"

#include "EASTL/hash_map.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/array.h"
#include "EASTL/map.h"
#include "EASTL/queue.h"
#include "EASTL/set.h"
#include "EASTL/stack.h"
#include "EASTL/unordered_set.h"



//-------------------------------------------------------------------------
#define InvalidIndex -1

namespace Lumina
{
    //-------------------------------------------------------------------------
    // Commonly used containers aliases
    //-------------------------------------------------------------------------

    template<typename T> using TVector =                                                eastl::vector<T>;
    template<typename T, eastl_size_t S, bool bOverflow = true> using TFixedVector =    eastl::fixed_vector<T, S, bOverflow>;
    template<typename T, eastl_size_t S> using TArray =                                 eastl::array<T, S>;
                                                                                        
    template<typename K, typename V> using TUnorderedMap =                              eastl::unordered_map<K, V>;
    template<typename K, typename V> using TOrderedMap =                                eastl::map<K, V>;
    template<typename K, typename V> using THashMap =                                   eastl::hash_map<K, V, eastl::hash<K>, eastl::equal_to<K>>;
    template<typename K, typename V> using TPair =                                      eastl::pair<K, V>;
    template<typename T> using TSet =                                                   eastl::set<T>;
    template<typename T> using TUnorderedSet =                                          eastl::unordered_set<T>;
                                                                                        
    template<typename T> using TQueue =                                                 eastl::queue<T>;
    template<typename T> using TDeque =                                                 eastl::deque<T>;
    template<typename T> using TStack =                                                 eastl::stack<T>;
    

    using Blob = TVector<uint8>;


    //-------------------------------------------------------------------------
    // Serialization for containers.
    //-------------------------------------------------------------------------

    template<typename ValueType>
    FArchive& operator << (FArchive& Ar, TVector<ValueType>& Array)
    {
		eastl_size_t SerializeNum = Ar.IsReading() ? 0 : Array.size();
        Ar << SerializeNum;
        
        if (SerializeNum == 0)
        {
            // if we are loading, then we have to reset the size to 0, in case it isn't currently 0
            if (Ar.IsReading())
            {
                Array.clear();
            }
            
            return Ar;
        }
        
        if (Ar.HasError() || SerializeNum > Ar.GetMaxSerializeSize())
        {
            return Ar;
        }

		// If we don't need to perform per-item serialization, just read it in bulk
        if constexpr (sizeof(ValueType) == 1 || TCanBulkSerialize<ValueType>::Value)
        {
            if (Ar.IsReading())
            {
                Array.resize(SerializeNum);
            }
            
            Ar.Serialize(Array.data(), SerializeNum * sizeof(ValueType));
        }
        else
        {
            if (Ar.IsReading())
            {
                Array.clear();
                Array.resize(SerializeNum);

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

    template<typename T, typename V>
    inline void VectorRemove(TVector<T>& Vector, const V& Value)
    {
        auto it = eastl::find(Vector.begin(), Vector.end(), Value);
        if (it != Vector.end())
        {
            Vector.erase(it);
        }
    }

    template<typename T>
    inline void VectorRemoveAtIndex(TVector<T>& Vector, uint32 Index)
    {
        Vector.erase(Vector.begin() + Index);
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
    inline bool VectorContains(TFixedVector<T, S> const& vector, V const& value)
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    template<typename T, eastl_size_t S, typename V, typename Predicate>
    inline bool VectorContains(TFixedVector<T, S> const& vector, V const& value, Predicate predicate)
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) ) != vector.end();
    }

    // Find an element in a vector
    template<typename T, typename V, eastl_size_t S>
    inline typename TFixedVector<T, S>::const_iterator VectorFind(TFixedVector<T, S> const& vector, V const& value)
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Find an element in a vector
    template<typename T, typename V, eastl_size_t S, typename Predicate>
    inline typename TFixedVector<T, S>::const_iterator VectorFind(TFixedVector<T, S> const& vector, V const& value, Predicate predicate)
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    // Find an element in a vector
    // Require non-const versions since we might want to modify the result
    template<typename T, typename V, eastl_size_t S>
    inline typename TFixedVector<T, S>::iterator VectorFind(TFixedVector<T, S>& vector, V const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Find an element in a vector
    // Require non-const versions since we might want to modify the result
    template<typename T, typename V, eastl_size_t S, typename Predicate>
    inline typename TFixedVector<T, S>::iterator VectorFind(TFixedVector<T, S>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    template<typename T, typename V, eastl_size_t S>
    inline int32_t VectorFindIndex(TFixedVector<T, S> const& vector, V const& value )
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
    inline int32_t VectorFindIndex(TFixedVector<T, S> const& vector, V const& value, Predicate predicate )
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
    inline void VectorEmplaceBackUnique(TFixedVector<T,S>& vector, T&& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }

    template<typename T, eastl_size_t S>
    inline void VectorEmplaceBackUnique(TFixedVector<T, S>& vector, T const& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }
}

#undef InvalidIndex