#pragma once
#include "Module/API.h"
#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{
    using RegisterFunc = void(*)();

    class FEntityComponentRegistry : public TSingleton<FEntityComponentRegistry>
    {
    public:

        LUMINA_API void AddDeferred(RegisterFunc fn)
        {
            Registrants.push_back(fn);
        }

        LUMINA_API void RegisterAll()
        {
            while (!Registrants.empty())
            {
                auto fn = Registrants.back();
                Registrants.pop_back();

                fn();
            }
        }

        void Clear()
        {
            Registrants.clear();
            Registrants.shrink_to_fit();
        }

    private:
        
        TFixedVector<RegisterFunc, 2024> Registrants;
    };
}
