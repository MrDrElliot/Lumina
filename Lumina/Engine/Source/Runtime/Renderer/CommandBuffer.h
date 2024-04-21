#pragma once
#include <memory>

namespace Lumina
{

    enum class ECommandBufferType
    {
        GENERAL,
        TRANSIENT
    };

    enum class ECommandType
    {
        GENERAL,
        ASYNC_COMPUTE,
        TRANSFER_DEDICATED
    };

    enum class ECommandBufferLevel
    {
        PRIMARY,
        SECONDARY
    };

    
    class FCommandBuffer
    {
    public:
        virtual ~FCommandBuffer() = default;

        static std::shared_ptr<FCommandBuffer> Create(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType);

        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Reset() = 0;
        virtual void Execute(bool bWait) = 0;
        virtual void Destroy() = 0;
    
    };
}
