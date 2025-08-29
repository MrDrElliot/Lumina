#pragma once

#include "Core/Object/ManualReflectTypes.h"
#include "ObjectBase.h"
#include "ObjectMacros.h"
#include "Core/Serialization/Archiver.h"
#include "Core/Serialization/Structured/StructuredArchive.h"


namespace Lumina
{
    class FProperty;
    class IStructuredArchive;
    class CClass;
    class CObject;
}

LUMINA_API Lumina::CClass* Construct_CClass_Lumina_CObject();

namespace Lumina
{

    //========================================================
    //
    // The base object for all reflected Lumina types.
    // Object paths are formatted as:
    //      <package-path>.<object-name>
    //=========================================================
    
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
        LUMINA_API CObject(CClass* InClass, EObjectFlags InFlags, CPackage* Package, const FName& InName)
            :CObjectBase(InClass, InFlags, Package, InName)
        {}

        /** Serializes object data. Can be overridden by derived classes. */
        LUMINA_API virtual void Serialize(FArchive& Ar);

        /** Serializes any reflected properties in this class */
        LUMINA_API virtual void SerializeReflectedProperties(FArchive& Ar);

        /** Used during serialization to and from a structured archive (Packaging, Network, etc.). */
        LUMINA_API virtual void Serialize(IStructuredArchive::FSlot Slot);
        
        /** Called after constructor and after properties have been initialized. */
        LUMINA_API virtual void PostInitProperties();

        /** Called after classes Class Default Object has been created */
        LUMINA_API virtual void PostCreateCDO() {}

        /** Called just before the object is serialized from disk */
        LUMINA_API virtual void PreLoad() {}
        
        /** Called immediately after the object is serialized from disk */
        LUMINA_API virtual void PostLoad() {}

        /** Called when a property on this object has been modified externally */
        LUMINA_API virtual void PostPropertyChange(FProperty* ChangedProperty) {}

        /** Renames this object, optionally changing it's package */
        LUMINA_API virtual bool Rename(const FName& NewName, CPackage* NewPackage = nullptr);
        
    private:

    };
}
