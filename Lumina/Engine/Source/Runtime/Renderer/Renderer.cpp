#include "Renderer.h"
#include "Assets/AssetHandle.h"
#include "Buffer.h"
#include "Image.h"
#include "PipelineLibrary.h"
#include "ShaderLibrary.h"
#include "Swapchain.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"
#include "Core/LuminaMacros.h"
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
        RenderAPI = new FVulkanRenderAPI(InConfig);
        
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

    void FRenderer::BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TFastVector<uint32>& DynamicOffsets)
    {
        RenderAPI->BindSet(Set, Pipeline, SetIndex, DynamicOffsets);
    }

    void FRenderer::Submit(RenderFunction&& Functor)
    {
        sInternalData.RenderFunctionList.push_back(std::move(Functor));
    }

    void FRenderer::BeginFrame()
    {
        sInternalData.NumDrawCalls = 0;
        sInternalData.NumVertices = 0;
        RenderAPI->BeginFrame();
    }

    void FRenderer::EndFrame()
    {
        RenderAPI->EndFrame();
    }

    void FRenderer::BeginRender(const TFastVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor)
    {
        RenderAPI->BeginRender(Attachments, ClearColor);
    }

    void FRenderer::EndRender()
    {
        RenderAPI->EndRender();
    }

    TRefPtr<FImage> FRenderer::GetSwapchainImage()
    {
        return RenderAPI->GetSwapchainImage();
    }

    void FRenderer::CopyToSwapchain(TRefPtr<FImage> ImageToCopy)
    {
        RenderAPI->CopyToSwapchain(ImageToCopy);
    }

    void FRenderer::BindPipeline(TRefPtr<FPipeline> Pipeline)
    {
        RenderAPI->BindPipeline(Pipeline);
    }

    TRefPtr<FSwapchain> FRenderer::GetSwapchain()
    {
        return RenderAPI->GetSwapchain();
    }

    void FRenderer::RenderMeshIndexed(TRefPtr<FPipeline> Pipeline, TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer, FMiscData Data)
    {
        sInternalData.NumDrawCalls++;
        RenderAPI->RenderMeshIndexed(Pipeline, VertexBuffer, IndexBuffer, Data);
    }

    void FRenderer::RenderVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
        sInternalData.NumDrawCalls++;
        sInternalData.NumVertices += Vertices;
        RenderAPI->RenderVertices(Vertices, Instances, FirstVertex, FirstInstance);
    }

    void FRenderer::RenderStaticMeshWithMaterial(const TRefPtr<FPipeline>& Pipeline, const std::shared_ptr<LStaticMesh>& StaticMesh, const TRefPtr<FMaterial>& Material)
    {
        sInternalData.NumDrawCalls++;
        sInternalData.NumVertices += StaticMesh->GetMeshData().Vertices.size();
        RenderAPI->RenderStaticMeshWithMaterial(Pipeline, StaticMesh, Material);
    }

    void FRenderer::RenderStaticMesh(const TRefPtr<FPipeline>& Pipeline, std::shared_ptr<LStaticMesh> StaticMesh, uint32 InstanceCount)
    {
        sInternalData.NumDrawCalls++;
        sInternalData.NumVertices += StaticMesh->GetMeshData().Vertices.size();
        RenderAPI->RenderStaticMesh(Pipeline, StaticMesh, InstanceCount);
    }

    void FRenderer::Render()
    {
        PROFILE_SCOPE(Render)

        // Submit rendering commands to the render function list
        FRenderer::Submit([]
        {
            TRefPtr<FImage> Image = GetSwapchainImage();

            Image->SetLayout(
                FRenderer::GetCommandBuffer(),
                EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE,
                EPipelineAccess::TRANSFER_WRITE
            );
        });

        ProcessRenderQueue();
    }

    void FRenderer::ProcessRenderQueue()
    {
        RenderAPI->EndCommandRecord();
        RenderAPI->ExecuteCurrentCommands();

        
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
        return RenderAPI->GetSwapchain()->GetCurrentFrameIndex();
    }

    void FRenderer::WaitIdle()
    {
        RenderAPI->WaitDevice();
    }

    void FRenderer::ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value)
    {
        RenderAPI->ClearColor(Image, Value);
    }

    void FRenderer::PushConstants(EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data)
    {
        RenderAPI->PushConstants(ShaderStage, Offset, Size, Data);
    }

    void FRenderer::RenderMeshTasks(TRefPtr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data)
    {
        RenderAPI->RenderMeshTasks(Pipeline, Dimensions, Data);
    }

    TRefPtr<FCommandBuffer> FRenderer::GetCommandBuffer()
    {
        return RenderAPI->GetCommandBuffer();
    }
}
