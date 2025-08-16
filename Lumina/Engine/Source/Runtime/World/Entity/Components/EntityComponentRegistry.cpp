#include "EntityComponentRegistry.h"

namespace Lumina
{
    void FEntityComponentRegistry::RegisterComponent(const char* Name, EntityComponentAddFn Fn)
    {
        for (const FRegistration& Registration : Registrations)
        {
            if (std::strcmp(Registration.Name, Name) == 0)
            {
                return;
            }
        }
        
        FRegistration Registration;
        Registration.Name = Name;
        Registration.Fn = std::move(Fn);
        Registrations.push_back(Registration);
    }

    EntityComponentAddFn FEntityComponentRegistry::GetComponentFn(const char* Name)
    {
        for (const FRegistration& Registration : Registrations)
        {
            if (std::strcmp(Registration.Name, Name) == 0)
            {
                return Registration.Fn;
            }
        }

        return nullptr;
    }
}
