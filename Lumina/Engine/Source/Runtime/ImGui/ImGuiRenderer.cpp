#include "ImGuiRenderer.h"
#include "ImGuiRenderAPI.h"
#include "Core/Layer.h"

namespace Lumina
{

    std::shared_ptr<FImGuiRenderAPI> FImGuiRenderer::RenderAPI = nullptr;
    
    FImGuiRenderer::FImGuiRenderer()
    {
    }

    FImGuiRenderer::~FImGuiRenderer()
    {
    }

    void FImGuiRenderer::Init()
    {
        RenderAPI = FImGuiRenderAPI::Create();
    }

    void FImGuiRenderer::Shutdown()
    {
        RenderAPI = nullptr;
    }

    void FImGuiRenderer::BeginFrame()
    {
        RenderAPI->BeginFrame();
    }

    void FImGuiRenderer::EndFrame()
    {
        RenderAPI->EndFrame();
    }


    void FImGuiRenderer::RenderImage(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer, bool bFlip)
    {
        RenderAPI->RenderImage(Image, Sampler, Size, ImageLayer, bFlip);
    }

    ImTextureID FImGuiRenderer::CreateImGuiTexture(const TRefPtr<FImage>& Image, const TRefPtr<FImageSampler>& Sampler, ImVec2 Size, uint32 ImageLayer, bool bFlip)
    {
        return RenderAPI->CreateImGuiTexture(Image, Sampler, Size, ImageLayer, bFlip);
    }
}
