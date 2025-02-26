#pragma once
#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Core/Math/Math.h"
#include "Types/BitFlags.h"
#include "Core/UpdateContext.h"

namespace Lumina
{
    struct FGPUBarrier;
}

namespace Lumina
{
    
    struct FCommandList
    {
        ECommandQueue CommandQueue;
        ECommandBufferUsage Type;
    };

    class IRenderContext
    {
    public:
        virtual ~IRenderContext() = default;

        virtual void Initialize() = 0;
        virtual void Deinitialize() = 0;

        virtual void FrameStart(const FUpdateContext& UpdateContext, uint8 CurrentFrameIndex) = 0;
        virtual void FrameEnd(const FUpdateContext& UpdateContext, uint8 CurrentFrameIndex) = 0;
        
        /**
         * 
         * @param CommandQueue Queue this command list uses.
         * @param Type Either a transient list or a general list.
         * @return A new, already recording command list.
         */
        virtual FCommandList* BeginCommandList(ECommandQueue CommandQueue = ECommandQueue::Graphics, ECommandBufferUsage Type = ECommandBufferUsage::General) = 0;
        virtual void EndCommandList(FCommandList* CommandList, bool bDestroy = false) = 0;
        
        virtual FRHIBufferHandle CreateBuffer(TBitFlags<ERenderDeviceBufferUsage> UsageFlags, TBitFlags<ERenderDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) = 0;
        virtual void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset = 0) = 0;
        virtual void CopyBuffer(FRHIBufferHandle Source, FRHIBufferHandle Destination) = 0;
        virtual uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<ERenderDeviceBufferUsage> Usage) = 0;

        virtual FRHIImageHandle CreateTexture(FIntVector2D Extent) = 0;
        virtual FRHIImageHandle CreateRenderTarget(FIntVector2D Extent) = 0;
        virtual FRHIImageHandle CreateDepthImage(FIntVector2D Extent) = 0;

        virtual void Barrier(FGPUBarrier* Barriers, uint32 BarrierNum, FCommandList* CommandList) = 0;


        //-------------------------------------------------------------------------------------

        
        virtual void BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass(FCommandList* CommandList) = 0;
    
    private:
        
    };
}
