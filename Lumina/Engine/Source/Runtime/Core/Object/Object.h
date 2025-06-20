﻿#pragma once

#include "ObjectBase.h"
#include "ObjectMacros.h"
#include "Core/Serialization/Archiver.h"
#include "Core/Serialization/Structured/StructuredArchive.h"
#include "Memory/Memory.h"

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

        DECLARE_CLASS(Lumina, CObject, CObject, "script://Lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CObject)

        LUMINA_API CObject();
        
        /** Internal constructor */
        LUMINA_API CObject(EObjectFlags InFlags)
            : CObjectBase(InFlags)
        {
        }

        /** Internal constructor */
        LUMINA_API CObject(CClass* InClass, EObjectFlags InFlags, const TCHAR* Package, FName InName)
            :CObjectBase(InClass, InFlags, Package, InName)
        {
        }

        /** Virtual destructor to allow proper cleanup in derived classes. */
        LUMINA_API virtual ~CObject() = default;

        /** Serializes object data. Can be overridden by derived classes. */
        LUMINA_API virtual void Serialize(FArchive& Ar) {}

        /** Used during serialization to and from a structured archive (Packaging, Network, etc). */
        LUMINA_API virtual void Serialize(IStructuredArchive::FSlot Slot);
        
        /** Called after constructor and after properites have been initialized. */
        LUMINA_API virtual void PostInitProperties();

        /** Called after classes Class Default Object has been created */
        LUMINA_API virtual void PostCreateCDO() { }
        
    private:

    };
}
