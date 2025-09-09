#pragma once
#include "Module/API.h"
#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{
    using RegisterFunc = void(*)();

    class FEntityComponentRegistry
    {
    public:

        LUMINA_API static FEntityComponentRegistry& Get();

        void AddDeferred(RegisterFunc fn);
        void RegisterAll();
        void Clear();

    private:
        
        TFixedVector<RegisterFunc, 100> Registrants;
    };
    

    namespace ECS
    {
        LUMINA_API void AddDeferredComponentRegistry(RegisterFunc Func);
    }
}
