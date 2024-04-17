#pragma once
#include <memory>

namespace Lumina
{
    class FCommandBuffer
    {
    public:

        static std::shared_ptr<FCommandBuffer> Create(bool bTransient);

        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Reset() = 0;
        virtual void Execute() = 0;
        virtual void Destroy() = 0;
    
    };
}
