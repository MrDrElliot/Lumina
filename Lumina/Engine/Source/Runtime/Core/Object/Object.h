#pragma once

#include "ObjectBase.h"
#include "ObjectMacros.h"
#include "Core/Serialization/Archiver.h"
#include "Core/Serialization/Structured/StructuredArchive.h"


namespace Lumina
{
    class IStructuredArchive;
}

LUMINA_API Lumina::CClass* Construct_CClass_Lumina_CObject();

namespace Lumina
{
    class CClass;
    class CObject;
    


    class CObject : public CObjectBase
    {
    public:

        friend CObject* StaticAllocateObject();

        DECLARE_CLASS(Lumina, CObject, CObject, "script://lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CObject)

        LUMINA_API CObject();
        
        /** Internal constructor */
        LUMINA_API CObject(EObjectFlags InFlags)
            : CObjectBase(InFlags)
        {}

        /** Internal constructor */
        LUMINA_API CObject(CClass* InClass, EObjectFlags InFlags, CPackage* Package, FName InName)
            :CObjectBase(InClass, InFlags, Package, InName)
        {}

        /** Virtual destructor to allow proper cleanup in derived classes. */
        LUMINA_API virtual ~CObject() = default;

        /** Serializes object data. Can be overridden by derived classes. */
        LUMINA_API virtual void Serialize(FArchive& Ar);

        /** Serializes any reflected properties in this class */
        LUMINA_API virtual void SerializeReflectedProperties(FArchive& Ar);

        /** Used during serialization to and from a structured archive (Packaging, Network, etc). */
        LUMINA_API virtual void Serialize(IStructuredArchive::FSlot Slot);
        
        /** Called after constructor and after properites have been initialized. */
        LUMINA_API virtual void PostInitProperties();

        /** Called after classes Class Default Object has been created */
        LUMINA_API virtual void PostCreateCDO() {}

        /** Allows any streamable resource to load in anything it needs, (e.g. an RHI Image). */
        LUMINA_API virtual void UpdateStreamableResource() {}
        
    private:

    };
}
