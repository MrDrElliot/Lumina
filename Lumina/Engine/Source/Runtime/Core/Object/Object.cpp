#include "Object.h"

#include "Class.h"

/** Low level CObject registration. */
extern Lumina::FClassRegistrationInfo Registration_Info_CClass_Lumina_CObject;
    
Lumina::CClass* Construct_CClass_Lumina_CObject()
{
    if (!Registration_Info_CClass_Lumina_CObject.Singleton)
    {
        Registration_Info_CClass_Lumina_CObject.Singleton = Lumina::CObject::StaticClass();
        Lumina::CObjectForceRegistration(Registration_Info_CClass_Lumina_CObject.Singleton);
    }
    Assert(Registration_Info_CClass_Lumina_CObject.Singleton->GetClass() != nullptr);
    return Registration_Info_CClass_Lumina_CObject.Singleton;
}
    
IMPLEMENT_CLASS(Lumina, CObject)

namespace Lumina
{

    CObject::CObject()
    {
        FObjectInitializer* Initializer = FObjectInitializer::Get();
        Initializer->Object = this;
    };

    CObject::CObject(ENoInit)
        :CObjectBase(NoInit)
    {
        //.. Zero init constructor.
    }

    void CObject::PostInitProperties()
    {
        
    }
}
