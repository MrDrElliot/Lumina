#pragma once

#include "Renderer.h"
#include "Image.h"

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
    
    class FRenderContext
    {
    public:

        FRenderContext() = default;
        virtual ~FRenderContext() = default;

        static FRenderContext* Create(const FRenderConfig& InConfig);
        virtual void Destroy() = 0;
        
        static TRefPtr<FImageSampler>& GetLinearSampler() { return Instance->LinearSampler; }
        static TRefPtr<FImageSampler>& GetNearestSampler() { return Instance->NearestSampler; }
        static FQueueFamilyIndex GetQueueFamilyIndex() { return Instance->QueueFamilyIndex; }

    
    protected:

        static FRenderContext* Instance;
        TRefPtr<FImageSampler> NearestSampler;
        TRefPtr<FImageSampler> LinearSampler;
        FQueueFamilyIndex QueueFamilyIndex;
        
    };
}
