#pragma once

#include "PipelineState.h"
#include "Renderer.h"
#include "RHIFwd.h"


namespace Lumina
{
    class FWindow;
    class FApplication;
    
    struct FQueueFamilyIndex 
    {
        uint32 Graphics;
        uint32 Transfer;
        uint32 Compute;
        uint32 Present;
    };
    
    class IRenderContext
    {
    public:

        IRenderContext()
            : QueueFamilyIndex()
        {}

        virtual ~IRenderContext() = default;

        virtual void Initialize() = 0;
        
        FQueueFamilyIndex GetQueueFamilyIndex() { return QueueFamilyIndex; }

        void SetCommandBufferForFrame(uint32 FrameIndex) { CurrentCommandBuffer = CommandBuffers[FrameIndex]; }
        FRHICommandBuffer GetCommandBuffer() { return CurrentCommandBuffer; }

        void SetCurrentPipeline(FRHIPipeline Pipeline);
        FORCEINLINE const FPipelineState* GetPipelineState() const { return &PipelineState; }

        template<typename T>
        TRefCountPtr<T> GetCommandBuffer()
        {
            return CurrentCommandBuffer.As<T>();
        }

        FRHISwapchain GetSwapchain() const { return Swapchain; }

        template<typename T>
        TRefCountPtr<T> GetSwapchain()
        {
            return Swapchain.As<T>();
        }
    
    
    protected:

        TVector<FRHICommandBuffer>        CommandBuffers;
        FRHICommandBuffer                 CurrentCommandBuffer;
        FQueueFamilyIndex                 QueueFamilyIndex;
        FRHISwapchain                     Swapchain;
        FPipelineState                    PipelineState;

        
    };
}
