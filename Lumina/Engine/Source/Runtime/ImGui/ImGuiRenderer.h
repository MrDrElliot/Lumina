#pragma once

#include <memory>
#include "imgui.h"
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FLayer;
    class FImageSampler;
    class FImage;
    class FImGuiRenderAPI;
    
    class FImGuiRenderer
    {
    public:

        FImGuiRenderer();
        virtual ~FImGuiRenderer();

        static void Init();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static void RenderImage(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer = 0, bool bFlip = false);
        static ImTextureID CreateImGuiTexture(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer = 0, bool bFlip = false);


    private:

        static TSharedPtr<FImGuiRenderAPI> RenderAPI;
    
    };
}
