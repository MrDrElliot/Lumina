#include "Renderer.h"

#include "Buffer.h"
#include "Image.h"
#include "PipelineLibrary.h"
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
        FShaderLibrary::Destroy();
        FPipelineLibrary::Shutdown();
        delete RenderAPI;
    }

    void FRenderer::LoadShaderPack()
    {
        FShaderLibrary::Get().Load("../Lumina/Engine/Resources/Shaders/Mesh.vert.spv",
            "../Lumina/Engine/Resources/Shaders/Mesh.frag.spv", "Mesh");
    }

    void FRenderer::BindSet(std::shared_ptr<FDescriptorSet> Set, std::shared_ptr<FPipeline> Pipeline, glm::uint8 Index)
    {
        RenderAPI->BindSet(Set, Pipeline, Index);
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

    void FRenderer::BeginRender(const std::vector<std::shared_ptr<FImage>> Attachments, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor)
    {
        RenderAPI->BeginRender(Attachments, RenderArea, RenderOffset, ClearColor);
    }

    void FRenderer::EndRender()
    {
        RenderAPI->EndRender();
    }

    std::shared_ptr<FImage> FRenderer::GetSwapchainImage()
    {
        return RenderAPI->GetSwapchainImage();
    }

    void FRenderer::CopyToSwapchain(std::shared_ptr<FImage> ImageToCopy)
    {
        RenderAPI->CopyToSwapchain(ImageToCopy);
    }

    void FRenderer::BindPipeline(std::shared_ptr<FPipeline> Pipeline)
    {
        RenderAPI->BindPipeline(Pipeline);
    }

    std::shared_ptr<FSwapchain> FRenderer::GetSwapchain()
    {
        return RenderAPI->GetSwapchain();
    }

    void FRenderer::RenderMeshIndexed(std::shared_ptr<FBuffer> VertexBuffer, std::shared_ptr<FBuffer> IndexBuffer)
    {
        RenderAPI->RenderMeshIndexed(VertexBuffer, IndexBuffer);
    }

    void FRenderer::RenderQuad(std::shared_ptr<FPipeline> Pipeline, FMiscData Data)
    {
        RenderAPI->RenderQuad(Pipeline, Data);
    }

    void FRenderer::RenderQuad(std::shared_ptr<FPipeline> Pipeline, glm::uint32 Amount, FMiscData Data)
    {
        RenderAPI->RenderQuad(Pipeline, Amount, Data);
    }

    void FRenderer::Render()
    {
        FRenderer::Submit([&]
        {
            auto Image = GetSwapchainImage();

            Image->SetLayout
            (
                FRenderer::GetCommandBuffer(),
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
            func();
        }
    }

    glm::uint32 FRenderer::GetCurrentFrameIndex()
    {
        return RenderAPI->GetSwapchain()->GetCurrentFrameIndex();
    }

    void FRenderer::RenderImGui()
    {
        RenderAPI->RenderImGui();
    }

    void FRenderer::ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value)
    {
        RenderAPI->ClearColor(Image, Value);
    }

    void FRenderer::RenderMeshTasks(std::shared_ptr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data)
    {
        RenderAPI->RenderMeshTasks(Pipeline, Dimensions, Data);
    }

    std::shared_ptr<FCommandBuffer> FRenderer::GetCommandBuffer()
    {
        return RenderAPI->GetCommandBuffer();
    }
}
