#include "Class.h"
#include "Object.h"
#include "Core/Reflection/Type/LuminaTypes.h"

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

    void CObject::Serialize(IStructuredArchive::FSlot Slot)
    {
        CClass* Class = GetClass();
        FName Name = GetName();

        Slot.Serialize(Name);

        Class->ForEachProperty([&](FProperty* Property)
        {
            Slot.GetStructuredArchive()->EnterField(Property->Name);
            Property->SerializeItem(Slot, this, nullptr);
            Slot.GetStructuredArchive()->LeaveField();
        });
        
    }

    void CObject::PostInitProperties()
    {
        
    }
}
