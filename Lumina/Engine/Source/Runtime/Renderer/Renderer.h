#pragma once
#include <cstdint>

namespace Lumina
{
    class FRenderer
    {
    public:

        FRenderer() = default;
        virtual ~FRenderer() = default;

        static void Init();
        static void Release();

        virtual void Begin() = 0;
        virtual void OnResize(uint32_t InWidth, uint32_t InHeight) = 0;

    protected:

        static FRenderer* Instance;
    
    };
}
