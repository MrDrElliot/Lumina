#pragma once
#include "World/Entity/EntityWorld.h"

namespace Lumina
{
    class CWorld;
    class FEntityWorld;
}

namespace Lumina
{
    /** Encapsulates the data a world system can execute */
    struct FSystemContext
    {
        friend class CWorld;
        
        FSystemContext(CWorld* World);
        ~FSystemContext();
        

        double GetDeltaTime() const { return DeltaTime; }
        
        template<typename... Ts, typename... TArgs>
        auto CreateView(TArgs&&... Args) -> decltype(std::declval<entt::registry>().view<Ts...>(std::forward<TArgs>(Args)...))
        {
            return EntityWorld->CreateView<Ts...>(std::forward<TArgs>(Args)...);
        }

        

    private:
        
        double DeltaTime = 0.0f;
        FEntityWorld* EntityWorld = nullptr;
    };
}
