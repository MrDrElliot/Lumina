#include "ImGuiRenderer.h"

#include "ImGuiRenderAPI.h"

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
        RenderAPI->Shutdown();
    }

    void FImGuiRenderer::BeginFrame()
    {
        RenderAPI->BeginFrame();
    }

    void FImGuiRenderer::EndFrame()
    {
        RenderAPI->EndFrame();
    }

    void FImGuiRenderer::AddLayer(std::shared_ptr<FLayer> InLayer)
    {
        RenderAPI->AddLayer(InLayer);
    }

    void FImGuiRenderer::RemoveLayer(std::shared_ptr<FLayer> InLayer)
    {
        RenderAPI->RemoveLayer(InLayer);
    }

    void FImGuiRenderer::Render(double DeltaTime)
    {
        RenderAPI->Render(DeltaTime);
    }

    void FImGuiRenderer::RenderImage(std::shared_ptr<FImage> Image, std::shared_ptr<FImageSampler> Sampler, ImVec2 Size, glm::uint32 ImageLayer, bool bFlip)
    {
        RenderAPI->RenderImage(Image, Sampler, Size, ImageLayer, bFlip);
    }
}
