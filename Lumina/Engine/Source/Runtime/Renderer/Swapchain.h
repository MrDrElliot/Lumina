#pragma once

#include "Renderer.h"


namespace Lumina
{

    struct FSwapchainSpec
    {
        FWindow* Window = nullptr;
        glm::ivec2 Extent;
        uint8 FramesInFlight;
    };
    
    class FSwapchain : public FRenderResource
    {
    public:

        FSwapchain()
            :Specifications({})
            , bDirty(false)
            , bWasResizedThisFrame(false)
        {}
        
        static TRefPtr<FSwapchain> Create(const FSwapchainSpec& InSpec);

        virtual void CreateSurface(IRenderContext* Context, const FSwapchainSpec& InSpec) = 0;
        virtual void CreateSwapchain(IRenderContext* Context, const FSwapchainSpec& InSpec) = 0;
        virtual void CreateImages() = 0;

        virtual void DestroySurface() = 0;
        virtual void DestroySwapchain() = 0;
        virtual void RecreateSwapchain() = 0;
        
        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        
        inline bool WasSwapchainResizedThisFrame() const { return bWasResizedThisFrame; }
        inline bool IsSwapchainDirty() const { return bDirty; }
        inline void SetSwapchainDirty() { bDirty = true; }

        TRefPtr<FImage>& GetCurrentImage() { return Images[CurrentImageIndex]; }
        
        uint32 GetCurrentFrameIndex() { return CurrentFrameIndex; }
        uint32 GetCurrentImageIndex() { return CurrentImageIndex; }
        FSwapchainSpec& GetSpecs() { return Specifications; }

    protected:
        
        FSwapchainSpec Specifications;
        TVector<TRefPtr<FImage>>  Images;

        uint8 bDirty:1;
        uint8 bWasResizedThisFrame:1;
        uint32 CurrentFrameIndex = 0;
        uint32 CurrentImageIndex = 0;

    };
}
