#pragma once

#include "ObjectBase.h"
#include "ObjectMacros.h"
#include "Core/Serialization/Archiver.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class CClass;
    class CObject;
    
    /**
     * Base class for all objects in Lumina, registered using DECLARE_CLASS or DECLARE_CLASS_ABSTRACT.
     * 
     * ## Overview:
     * - "C" Prefix is standard it implies "Class", all objects derived from CObject should be prefixed with "C".
     * - All objects are tracked in a global object vector.
     * - CObjects **must** have a default constructor; constructor properties are not supported.
     * - Only default-generated constructors are allowed. (No params).
     */
    
    class CObject : public CObjectBase
    {
    public:

        DECLARE_CLASS(CObject, CObject, LUMINA_API)
        
        /** Internal constructor */
        LUMINA_API CObject(EObjectFlags InFlags)
            : CObjectBase(InFlags)
        {
        }

        /** Internal constructor */
        LUMINA_API CObject(CClass* InClass, EObjectFlags InFlags, FName InName)
            :CObjectBase(InClass, InFlags, InName)
        {
        }

        /** Virtual destructor to allow proper cleanup in derived classes. */
        LUMINA_API virtual ~CObject() = default;

        /** Serializes object data. Can be overridden by derived classes. */
        LUMINA_API virtual void Serialize(FArchive& Ar) {}
    
        
    
        
    private:

    };
}
