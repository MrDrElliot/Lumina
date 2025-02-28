#pragma once
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class IRenderContext;
}

namespace Lumina
{
    enum class ECommandBufferLevel : uint8;
    enum class ECommandBufferUsage : uint8;
    enum class ECommandQueue : uint8;

    class ICommandList
    {
    public:
        
        virtual ~ICommandList() = default;

        virtual void Begin() = 0;
        virtual void FlushCommandList() = 0;
        virtual void SubmitCommandList() = 0;
        virtual void Reset() = 0;
        virtual void Destroy() = 0;

        ECommandQueue CommandQueue;
        ECommandBufferUsage Type;
        ECommandBufferLevel Level;
    };
}
