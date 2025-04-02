#pragma once

#include "Class.h"
#include "ObjectMacros.h"
#include "ObjectFlags.h"
#include "Core/Functional/Function.h"
#include "Core/Serialization/Archiver.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class CObject;

    inline TVector<CObject*> GObjectVector;
    
    /**
     * Base class for all objects in Lumina, registered using DECLARE_CLASS or DECLARE_CLASS_ABSTRACT.
     * 
     * ## Overview:
     * - "C" Prefix is standard it implies "Class", all objects derived from CObject should be prefixed with "C".
     * - All objects are tracked in a global object vector.
     * - CObjects **must** have a default constructor; constructor properties are not supported.
     * - Only default-generated constructors are allowed. (No params).
     */
    LUM_CLASS()
    class CObject
    {
    public:
        
        /** Default constructor. Required for all CObjects. */
        CObject() = default;

        /** Virtual destructor to allow proper cleanup in derived classes. */
        virtual ~CObject() = default;

        /** Serializes object data. Can be overridden by derived classes. */
        virtual void Serialize(FArchive& Ar) {}

        /** Returns the class type name. Must be implemented by derived classes. */
        virtual Class* GetClass() const = 0;
        

        FORCEINLINE EObjectFlags GetFlags() const { return ObjectFlags; }
        FORCEINLINE bool HasAnyFlag(EObjectFlags Flag) { return EnumHasAnyFlags(ObjectFlags, Flag); }
        FORCEINLINE bool HasAllFlags(EObjectFlags Flags) { return EnumHasAllFlags(ObjectFlags, Flags); }
        

        // DO NOT USE.
        void SetGlobalObjectEntryElement(uint32 Index) { ObjectIndex = Index; }
        uint32 GlobalObjectIndex() const { return ObjectIndex; }
        
    private:

        EObjectFlags ObjectFlags;
        uint32 ObjectIndex = 0;
    };
    
    inline void DeleteObject(CObject* Obj)
    {
        Assert(Obj != nullptr);
        VectorRemoveAtIndex(GObjectVector, Obj->GlobalObjectIndex());
        FMemory::Delete(Obj);
    }

    
    template <typename T>
    concept IsValidCObject = std::is_base_of_v<CObject, T> && !std::is_abstract_v<T>;
    
    template<IsValidCObject T>
    inline T* NewObject(Class* ClassID)
    {

        CObject* Obj = FClassRegistry::GetInstance().CreateInstance(ClassID->ID);
        if (Obj)
        {
            Obj->SetGlobalObjectEntryElement(GObjectVector.size() - 1);
            return static_cast<T*>(Obj);
        }

        return nullptr;
    }

    template<IsValidCObject T>
    inline T* NewObject()
    {
        
        CObject* Obj = FClassRegistry::GetInstance().CreateInstance(T::StaticClass()->ID);
        if (Obj)
        {
            GObjectVector.emplace_back(Obj);
            Obj->SetGlobalObjectEntryElement(GObjectVector.size() - 1);
            return static_cast<T*>(Obj);
        }

        return nullptr;
    }
}
