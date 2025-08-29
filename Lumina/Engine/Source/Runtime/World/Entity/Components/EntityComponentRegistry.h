#pragma once
#include "Module/API.h"
#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{
    using RegisterFunc = void(*)();

    class LUMINA_API FEntityComponentRegistry : public TSingleton<FEntityComponentRegistry>
    {
    public:

        void AddDeferred(RegisterFunc fn)
        {
            Registrants.push_back(fn);
        }

        void RegisterAll()
        {
            while (!Registrants.empty())
            {
                auto fn = Registrants.back();
                Registrants.pop_back();

                fn();
            }
        }

    private:
        
        TVector<RegisterFunc> Registrants;
    };
}
