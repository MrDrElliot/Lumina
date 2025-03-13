#pragma once
#include "Singleton/Singleton.h"

namespace Lumina
{
    class FClassRegistry : public TSingleton<FClassRegistry>
    {
    public:

        template<typename T>
        void RegisterClass()
        {
            
        }

    private:
    
    };
}
 