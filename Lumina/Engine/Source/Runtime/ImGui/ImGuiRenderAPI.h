#pragma once
#include <memory>

#include "imgui.h"
#include <glm/glm.hpp>

#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FImageSampler;
    class FImage;
    class FLayer;

    class FImGuiRenderAPI
    {
    public:
        
        virtual ~FImGuiRenderAPI() = default;

        static TSharedPtr<FImGuiRenderAPI> Create();

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        
        virtual void RenderImage(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer = 0, bool bFlip = false) = 0;
        virtual ImTextureID CreateImGuiTexture(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer = 0, bool bFlip = false) = 0;

    
    };
}
