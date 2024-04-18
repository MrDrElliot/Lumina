#include "Renderer.h"

#include "Image.h"
#include "ShaderLibrary.h"
#include "RHI/Vulkan/VulkanRenderAPI.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    FRenderAPI* FRenderer::RenderAPI = nullptr;

    struct RendererInternalData
    {
        std::list<FRenderer::RenderFunction> FunctorList;
        std::shared_ptr<FImageSampler> LinearSampler;
        std::shared_ptr<FImageSampler> NearestSampler;
    } InternalData;
    
    void FRenderer::Init(const FRenderConfig& InConfig)
    {
        LE_LOG_INFO("Renderer: Initializing");
        RenderAPI = new FVulkanRenderAPI(InConfig);
        FShaderLibrary::Init();

        LoadShaderPack();
    }
    
    void FRenderer::Shutdown()
    {
        LE_LOG_WARN("Renderer: Shutting Down");
        delete RenderAPI;
        FShaderLibrary::Destroy();
    }

    void FRenderer::LoadShaderPack()
    {
        FShaderLibrary::Get()->Load("Resources/");
    }

    void FRenderer::Submit(RenderFunction Functor)
    {
        InternalData.FunctorList.push_back(Functor);
    }

    void FRenderer::BeginFrame()
    {
        RenderAPI->BeginFrame();
    }

    void FRenderer::EndFrame()
    {
        RenderAPI->EndFrame();
    }

    void FRenderer::BeginRender(std::shared_ptr<FImage> Target, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor)
    {
        RenderAPI->BeginRender(Target, RenderArea, RenderOffset, ClearColor);
    }

    void FRenderer::EndRender()
    {
        RenderAPI->EndRender();
    }

    std::shared_ptr<FImage> FRenderer::GetSwapchainImage()
    {
        return RenderAPI->GetSwapchainImage();
    }

    void FRenderer::Render()
    {
        FRenderer::Submit([&] (std::shared_ptr<FCommandBuffer> CmdBuffer)
        {
            auto Image = GetSwapchainImage();

            Image->SetLayout
            (
                        CmdBuffer,
                        EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE,
                EPipelineAccess::TRANSFER_WRITE
            );
        });
        
        RenderAPI->EndCommandRecord();
        RenderAPI->ExecuteCurrentCommands();

     
        auto List = std::move(InternalData.FunctorList);
        
        for(auto& func : List)
        {
            func(GetCommandBuffer());
        }
    }

    void FRenderer::ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value)
    {
        RenderAPI->ClearColor(Image, Value);
    }

    std::shared_ptr<FCommandBuffer> FRenderer::GetCommandBuffer()
    {
        return RenderAPI->GetCommandBuffer();
    }
}
