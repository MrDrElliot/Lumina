#pragma once

#include "RenderResource.h"
#include "Memory/RefCounted.h"


namespace Lumina
{

    enum class ECommandBufferType
    {
        GENERAL,
        TRANSIENT,
        MAX,
    };

    enum class ECommandType
    {
        GENERAL,
        ASYNC_COMPUTE,
        TRANSFER_DEDICATED,
        MAX,
    };

    enum class ECommandBufferLevel
    {
        PRIMARY,
        SECONDARY,
        MAX,
    };

    
    class FCommandBuffer : public FRenderResource
    {
    public:
        virtual ~FCommandBuffer() = default;

        static TRefPtr<FCommandBuffer> Create(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType);
        
        FCommandBuffer(ECommandBufferLevel InLevel, ECommandBufferType InBufferType, ECommandType InCmdType)
            : Level(InLevel)
            , BufferType(InBufferType)
            , CmdType(InCmdType)
        {}
        
        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Reset() = 0;
        virtual void Execute(bool bWait) = 0;


    protected:
        
        ECommandBufferLevel         Level = ECommandBufferLevel::MAX;
        ECommandBufferType          BufferType = ECommandBufferType::MAX;
        ECommandType                CmdType = ECommandType::MAX;
    
    };
}
