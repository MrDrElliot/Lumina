#pragma once

#include "ObjectBase.h"
#include "ObjectMacros.h"
#include "Core/Serialization/Archiver.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class CClass;
    class CObject;
    

    LUMINA_API CClass* Construct_CClass_Lumina_CObject();

    class CObject : public CObjectBase
    {
    public:

        friend CObject* StaticAllocateObject();

        DECLARE_CLASS(Lumina, CObject, CObject, TEXT("Script/Engine"), LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CObject)

        LUMINA_API CObject();
        LUMINA_API CObject(ENoInit);
        
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

        /** Called after constructor and after properites have been initialized. */
        LUMINA_API virtual void PostInitProperties();

        /** Called after classes Class Default Object has been created */
        LUMINA_API virtual void PostCreateCDO() { }
        
    private:

    };
}
