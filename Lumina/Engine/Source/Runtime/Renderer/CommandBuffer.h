#pragma once

#include "RenderResource.h"
#include "Memory/RefCounted.h"


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

    
    class FCommandBuffer : public FRenderResource
    {
    public:
        virtual ~FCommandBuffer() = default;

        static TRefPtr<FCommandBuffer> Create(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType);
        
        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Reset() = 0;
        virtual void Execute(bool bWait) = 0;
    
    };
}
