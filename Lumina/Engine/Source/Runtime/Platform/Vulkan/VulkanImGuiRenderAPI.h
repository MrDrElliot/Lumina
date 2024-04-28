#pragma once
#include "Source/Runtime/ApplicationCore/LayerStack.h"
#include "Source/Runtime/ImGui/ImGuiRenderAPI.h"

namespace Lumina
{
    class FVulkanImGuiRenderAPI : public FImGuiRenderAPI
    {
    public:

        FVulkanImGuiRenderAPI();
        ~FVulkanImGuiRenderAPI();

        void BeginFrame() override;
        void EndFrame() override;

        void Init() override;
        void Shutdown() override;

        void Render(double DeltaTime) override;

        void AddLayer(std::shared_ptr<FLayer> InLayer) override;
        void RemoveLayer(std::shared_ptr<FLayer> InLayer) override;
        void RenderImage(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer, bool bFlip) override;
        virtual ImTextureID CreateImGuiTexture(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer = 0, bool bFlip = false);


    private:

        FLayerStack Layers;
    
    };
}
