#pragma once
#include <memory>

#include "imgui.h"
#include <glm/glm.hpp>

namespace Lumina
{
    class FImageSampler;
    class FImage;
    class FLayer;

    class FImGuiRenderAPI
    {
    public:
        virtual ~FImGuiRenderAPI() = default;

        static std::shared_ptr<FImGuiRenderAPI> Create();

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void Render(double DeltaTime) = 0;

        virtual void AddLayer(std::shared_ptr<FLayer> InLayer) = 0;
        virtual void RemoveLayer(std::shared_ptr<FLayer> InLayer) = 0;
        virtual void RenderImage(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer = 0, bool bFlip = false) = 0;
        virtual ImTextureID CreateImGuiTexture(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer = 0, bool bFlip = false) = 0;

    
    };
}
