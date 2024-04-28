#pragma once
#include <memory>

#include "Source/Runtime/ApplicationCore/LayerStack.h"

#include <glm/glm.hpp>

#include "imgui.h"

namespace Lumina
{
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

        static void AddLayer(std::shared_ptr<FLayer> InLayer);
        static void RemoveLayer(std::shared_ptr<FLayer> InLayer);

        static void Render(double DeltaTime);
        static void RenderImage(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer = 0, bool bFlip = false);
        static ImTextureID CreateImGuiTexture(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer = 0, bool bFlip = false);


    private:

        static std::shared_ptr<FImGuiRenderAPI> RenderAPI;
    
    };
}
