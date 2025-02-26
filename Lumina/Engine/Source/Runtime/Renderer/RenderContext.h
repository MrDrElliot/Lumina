#pragma once
#include "RenderResource.h"
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
    
    class FCommandList
    {
    public:
        
        ECommandQueue CommandQueue;
        ECommandBufferUsage Type;
        ECommandBufferLevel Level;
    };

    class IRenderContext
    {
    public:
        virtual ~IRenderContext() = default;

        virtual void Initialize() = 0;
        virtual void Deinitialize() = 0;

        virtual void WaitIdle() = 0;

        virtual void SetVSyncEnabled(bool bEnable) = 0;
        virtual bool IsVSyncEnabled() const = 0;

        virtual void FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) = 0;
        virtual void FrameEnd(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) = 0;
        
        virtual FCommandList* BeginCommandList(ECommandBufferLevel Level = ECommandBufferLevel::Secondary, ECommandQueue CommandType = ECommandQueue::Graphics, ECommandBufferUsage Usage = ECommandBufferUsage::General) = 0;
        virtual void EndCommandList(FCommandList* CommandList) = 0;
        
        virtual FRHIBufferHandle CreateBuffer(TBitFlags<ERenderDeviceBufferUsage> UsageFlags, TBitFlags<ERenderDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) = 0;
        virtual void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset = 0) = 0;
        virtual void CopyBuffer(FRHIBufferHandle Source, FRHIBufferHandle Destination) = 0;
        virtual uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<ERenderDeviceBufferUsage> Usage) = 0;

        virtual FRHIImageHandle AllocateImage() = 0;
        virtual FRHIImageHandle CreateTexture(const FImageSpecification& ImageSpec) = 0;
        virtual FRHIImageHandle CreateRenderTarget(const FIntVector2D& Extent) = 0;
        virtual FRHIImageHandle CreateDepthImage(const FImageSpecification& ImageSpec) = 0;

        virtual void Barrier(FGPUBarrier* Barriers, uint32 BarrierNum, FCommandList* CommandList) = 0;


        //-------------------------------------------------------------------------------------

        
        virtual void BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass(FCommandList* CommandList) = 0;

        virtual void ClearColor(FCommandList* CommandList, const FColor& Color) = 0;
    
    private:
        
    };
}
