#include "Object.h"

#include "Class.h"

namespace Lumina
{
    /** Low level CObject registration. */
    extern FClassRegistrationInfo Registration_Info_CClass_Lumina_CObject;
    
    CClass* Construct_CClass_Lumina_CObject()
    {
        if (!Registration_Info_CClass_Lumina_CObject.Singleton)
        {
            Registration_Info_CClass_Lumina_CObject.Singleton = CObject::StaticClass();
            CObjectForceRegistration(Registration_Info_CClass_Lumina_CObject.Singleton);
        }
        Assert(Registration_Info_CClass_Lumina_CObject.Singleton->GetClass() != nullptr);
        return Registration_Info_CClass_Lumina_CObject.Singleton;
    }
    
    IMPLEMENT_CLASS(Lumina, CObject)

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
