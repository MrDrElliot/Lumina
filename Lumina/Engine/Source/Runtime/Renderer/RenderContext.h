#pragma once

#include "Pipeline.h"
#include "Renderer.h"
#include "Image.h"
#include "Swapchain.h"


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

        IRenderContext(const FRenderConfig& InConfig)
            : Config(InConfig)
            , QueueFamilyIndex()
        {}

        virtual ~IRenderContext() = default;

        virtual void Initialize() = 0;
        
        FQueueFamilyIndex GetQueueFamilyIndex() { return QueueFamilyIndex; }

        void SetCommandBufferForFrame(uint32 FrameIndex) { CurrentCommandBuffer = CommandBuffers[FrameIndex]; }
        TRefPtr<FCommandBuffer> GetCommandBuffer() { return CurrentCommandBuffer; }
        

        template<typename T>
        TRefPtr<T> GetCommandBuffer()
        {
            return RefPtrCast<T>(CurrentCommandBuffer);
        }

        TRefPtr<FSwapchain> GetSwapchain() const { return Swapchain; }

        template<typename T>
        TRefPtr<T> GetSwapchain()
        {
            return RefPtrCast<T>(Swapchain);
        }
    
    
    protected:

        FRenderConfig                     Config;
        TVector<TRefPtr<FCommandBuffer>>  CommandBuffers;
        TRefPtr<FCommandBuffer>           CurrentCommandBuffer;
        FQueueFamilyIndex                 QueueFamilyIndex;
        TRefPtr<FSwapchain>               Swapchain;
        TRefPtr<FPipeline>                CurrentBoundPipeline;
        
    };
}
