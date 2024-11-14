#pragma once

#include "Core/LayerStack.h"
#include "ImGui/ImGuiRenderAPI.h"

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
        
        void RenderImage(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer, bool bFlip) override;
        virtual ImTextureID CreateImGuiTexture(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer = 0, bool bFlip = false);

        
    };
}
