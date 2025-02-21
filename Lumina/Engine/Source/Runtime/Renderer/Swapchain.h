#pragma once

#include "Renderer.h"
#include "Core/Math/Math.h"


namespace Lumina
{

    struct FSwapchainSpec
    {
        const FWindow* Window = nullptr;
        FVector2D Extent;
    };
    
    class FSwapchain : public IRenderResource
    {
    public:

        FSwapchain()
            :Specifications({})
            , bDirty(false)
            , bWasResizedThisFrame(false)
        {}
        
        static FRHISwapchain Create(const FSwapchainSpec& InSpec);

        virtual void CreateSurface(IRenderContext* Context, const FSwapchainSpec& InSpec) = 0;
        virtual void CreateSwapchain(IRenderContext* Context, const FSwapchainSpec& InSpec) = 0;
        virtual void CreateImages() = 0;

        virtual void DestroySurface() = 0;
        virtual void DestroySwapchain() = 0;
        virtual void RecreateSwapchain() = 0;
        
        virtual bool BeginFrame() = 0;
        virtual void Present() = 0;
        
        inline bool WasSwapchainResizedThisFrame() const { return bWasResizedThisFrame; }
        inline bool IsSwapchainDirty() const { return bDirty; }
        inline void SetSwapchainDirty() { bDirty = true; }

        FRHIImage& GetCurrentImage() { return Images[CurrentImageIndex]; }
        
        uint32 GetCurrentFrameIndex() { return CurrentFrameIndex; }
        uint32 GetCurrentImageIndex() { return CurrentImageIndex; }
        FSwapchainSpec& GetSpecs() { return Specifications; }

    protected:
        
        FSwapchainSpec Specifications;
        TVector<FRHIImage>  Images;

        uint8 bDirty:1;
        uint8 bWasResizedThisFrame:1;
        uint32 CurrentFrameIndex = 0;
        uint32 CurrentImageIndex = 0;

    };
}
