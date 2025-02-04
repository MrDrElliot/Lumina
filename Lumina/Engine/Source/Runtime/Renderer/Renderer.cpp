#include "Renderer.h"
#include "Assets/AssetHandle.h"
#include "Buffer.h"
#include "Image.h"
#include "PipelineLibrary.h"
#include "ShaderLibrary.h"
#include "Swapchain.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Paths/Paths.h"
#include "RHI/Vulkan/VulkanRenderAPI.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    IRenderAPI* FRenderer::RenderAPI = nullptr;
    FRenderer::RendererInternalData FRenderer::sInternalData;
    
    void FRenderer::Init(const FRenderConfig& InConfig)
    {
        LOG_TRACE("Renderer: Initializing");
        RenderAPI = IRenderAPI::Create(InConfig);
        RenderAPI->Initialize(InConfig);
        
        // Nearest filtration sampler
        FImageSamplerSpecification ImageSpec = {};
        ImageSpec.MinFilteringMode =            ESamplerFilteringMode::LINEAR;
        ImageSpec.MagFilteringMode =            ESamplerFilteringMode::NEAREST;
        ImageSpec.MipMapFilteringMode =         ESamplerFilteringMode::LINEAR;
        ImageSpec.AddressMode =                 ESamplerAddressMode::REPEAT;
        ImageSpec.MinLOD =                      0.0f;
        ImageSpec.MaxLOD =                      1000.0f;
        ImageSpec.LODBias =                     0.0f;
        ImageSpec.AnisotropicFilteringLevel =   1;

        sInternalData.NearestSampler = FImageSampler::Create(ImageSpec);
        sInternalData.NearestSampler->SetFriendlyName("Nearest Sampler");
        
        // Linear filtration sampler
        ImageSpec.MinFilteringMode =            ESamplerFilteringMode::LINEAR;
        ImageSpec.MagFilteringMode =            ESamplerFilteringMode::LINEAR;
        ImageSpec.MipMapFilteringMode =         ESamplerFilteringMode::LINEAR;
        ImageSpec.AddressMode =                 ESamplerAddressMode::REPEAT;
        ImageSpec.MinLOD =                      0.0f;
        ImageSpec.MaxLOD =                      1000.0f;
        ImageSpec.LODBias =                     0.0f;
        ImageSpec.AnisotropicFilteringLevel =   1;

        sInternalData.LinearSampler = FImageSampler::Create(ImageSpec);
        sInternalData.LinearSampler->SetFriendlyName("Linear Sampler");
        
        LoadShaderPack();
    }
    
    void FRenderer::Shutdown()
    {
        LOG_TRACE("Renderer: Shutting Down");
        WaitIdle();
        sInternalData.LinearSampler->Destroy();
        sInternalData.NearestSampler->Destroy();
        sInternalData.RenderFunctionList.clear();
        FShaderLibrary::Get()->Shutdown();
        FPipelineLibrary::Get()->Shutdown();
        delete RenderAPI;
    }

    void FRenderer::LoadShaderPack()
    {
        FShaderLibrary::Get()->Load(Paths::GetEngineDirectory() / "Resources/Shaders/CookTorrance.vert.spv",
            Paths::GetEngineDirectory() / "Resources/Shaders/CookTorrance.frag.spv", "Mesh");

        FShaderLibrary::Get()->Load(Paths::GetEngineDirectory() / "Resources/Shaders/InfiniteGrid.vert.spv",
      Paths::GetEngineDirectory() / "Resources/Shaders/InfiniteGrid.frag.spv", "InfiniteGrid");
        
    }

    TRefPtr<FImageSampler> FRenderer::GetLinearSampler()
    {
        return sInternalData.LinearSampler;
    }

    TRefPtr<FImageSampler> FRenderer::GetNearestSampler()
    {
        return sInternalData.NearestSampler;
    }

    void FRenderer::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
        RenderAPI->InsertBarrier(BarrierInfo);
    }

    void FRenderer::BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets)
    {
        RenderAPI->BindSet(Set, Pipeline, SetIndex, DynamicOffsets);
    }

    void FRenderer::Submit(RenderFunction&& Functor)
    {
        sInternalData.RenderFunctionList.push_back(std::move(Functor));
    }

    void FRenderer::BeginFrame()
    {
    }

    void FRenderer::EndFrame()
    {
        RenderAPI->EndFrame();
    }

    void FRenderer::BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor)
    {
        RenderAPI->BeginRender(Attachments, ClearColor);
    }

    void FRenderer::EndRender()
    {
        RenderAPI->EndRender();
    }
    
    void FRenderer::CopyToSwapchain(TRefPtr<FImage> ImageToCopy)
    {
        RenderAPI->CopyToSwapchain(ImageToCopy);
    }

    void FRenderer::BindPipeline(TRefPtr<FPipeline> Pipeline)
    {
        RenderAPI->BindPipeline(Pipeline);
    }

    IRenderContext* FRenderer::GetRenderContext()
    {
        return RenderAPI->GetRenderContext();
    }
    
    void FRenderer::DrawIndexed(TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer)
    {
        sInternalData.NumDrawCalls++;
        RenderAPI->DrawIndexed(VertexBuffer, IndexBuffer);
    }

    void FRenderer::DrawVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
        sInternalData.NumDrawCalls++;
        sInternalData.NumVertices += Vertices;
        RenderAPI->DrawVertices(Vertices, Instances, FirstVertex, FirstInstance);
    }


    void FRenderer::Update()
    {
        PROFILE_SCOPE(Renderer_Update)

        sInternalData.NumDrawCalls = 0;
        sInternalData.NumVertices = 0;

        RenderAPI->BeginFrame();
        ProcessRenderQueue();
        RenderAPI->EndFrame();

    }

    void FRenderer::ProcessRenderQueue()
    {
        FRenderer::Submit([]
        {
            TRefPtr<FImage> Image = GetRenderContext()->GetSwapchain()->GetCurrentImage();
        
            Image->SetLayout(
                FRenderer::GetCommandBuffer(),
                EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE,
                EPipelineAccess::TRANSFER_WRITE
            );
        });
        
        for (auto& func : sInternalData.RenderFunctionList)
        {
            func();
        }
        
        sInternalData.RenderFunctionList.clear();
        
    }

    FRenderConfig FRenderer::GetConfig()
    {
        return RenderAPI->GetConfig();
    }

    uint32 FRenderer::GetCurrentFrameIndex()
    {
        return RenderAPI->GetRenderContext()->GetSwapchain()->GetCurrentFrameIndex();
    }

    void FRenderer::WaitIdle()
    {
        RenderAPI->WaitDevice();
    }

    void FRenderer::ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value)
    {
        RenderAPI->ClearColor(Image, Value);
    }

    void FRenderer::PushConstants(TRefPtr<FPipeline> Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data)
    {
        RenderAPI->PushConstants(Pipeline, ShaderStage, Offset, Size, Data);
    }
    
    TRefPtr<FCommandBuffer> FRenderer::GetCommandBuffer()
    {
        return GetRenderContext()->GetCommandBuffer();
    }
}
