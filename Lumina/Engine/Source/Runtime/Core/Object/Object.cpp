﻿#include "Class.h"
#include "Object.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Core/Reflection/Type/Properties/ArrayProperty.h"

/** Low level CObject registration. */
extern Lumina::FClassRegistrationInfo Registration_Info_CClass_Lumina_CObject;
    
Lumina::CClass* Construct_CClass_Lumina_CObject()
{
    if (!Registration_Info_CClass_Lumina_CObject.OuterSingleton)
    {
        Registration_Info_CClass_Lumina_CObject.OuterSingleton = Lumina::CObject::StaticClass();
        Lumina::CObjectForceRegistration(Registration_Info_CClass_Lumina_CObject.OuterSingleton);
    }
    Assert(Registration_Info_CClass_Lumina_CObject.OuterSingleton->GetClass() != nullptr)
    return Registration_Info_CClass_Lumina_CObject.OuterSingleton;
}
    
IMPLEMENT_CLASS(Lumina, CObject)

namespace Lumina
{

    CObject::CObject()
    {
        FObjectInitializer* Initializer = FObjectInitializer::Get();
        Initializer->Object = this;
    };

    void CObject::Serialize(FArchive& Ar)
    {
        CClass* Class = GetClass();
        if (Class)
        {
            SerializeReflectedProperties(Ar);
        }
    }

    void CObject::SerializeReflectedProperties(FArchive& Ar)
    {
        GetClass()->SerializeClassProperties(Ar, this);
    }

    void CObject::Serialize(IStructuredArchive::FSlot Slot)
    {
        CClass* Class = GetClass();
        FName Name = GetName();

        Slot.Serialize(Name);
        
        FProperty* Current = Class->LinkedProperty;
        while (Current != nullptr)
        {
            Slot.GetStructuredArchive()->EnterField(Current->Name);

            // Gets the address of the actual property.
            void* ValuePtr = Current->GetValuePtr<void>(this);
            Current->SerializeItem(Slot, ValuePtr, nullptr);
            
            Slot.GetStructuredArchive()->LeaveField();

            Current = (FProperty*)Current->Next;
        }

        Serialize(*Slot.GetStructuredArchive()->GetInnerAr());
    }

    void CObject::PostInitProperties()
    {
        
    }
}
