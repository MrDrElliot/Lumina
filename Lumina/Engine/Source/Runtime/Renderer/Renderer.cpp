#include "Renderer.h"
#include "Assets/AssetHandle.h"
#include "Buffer.h"
#include "Image.h"
#include "PipelineLibrary.h"
#include "ShaderLibrary.h"
#include "Swapchain.h"
#include "RHI/Vulkan/VulkanRenderAPI.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    IRenderAPI* FRenderer::RenderAPI = nullptr;

    struct RendererInternalData
    {
        std::list<FRenderer::RenderFunction> RenderFunctionList;
        TRefPtr<FImageSampler> LinearSampler;
        TRefPtr<FImageSampler> NearestSampler;
    } InternalData;
    
    void FRenderer::Init(const FRenderConfig& InConfig)
    {
        LOG_TRACE("Renderer: Initializing");
        RenderAPI = new FVulkanRenderAPI(InConfig);
        LoadShaderPack();
    }
    
    void FRenderer::Shutdown()
    {
        LOG_TRACE("Renderer: Shutting Down");
        WaitIdle();
        //InternalData.LinearSampler->Destroy();
        //InternalData.NearestSampler->Destroy();
        InternalData.RenderFunctionList.clear();
        FShaderLibrary::Get()->Shutdown();
        FPipelineLibrary::Get()->Shutdown();
        RenderAPI->Shutdown();
        delete RenderAPI;
    }

    void FRenderer::LoadShaderPack()
    {
        FShaderLibrary::Get()->Load("../Lumina/Engine/Resources/Shaders/CookTorrance.vert.spv",
            "../Lumina/Engine/Resources/Shaders/CookTorrance.frag.spv", "Mesh");

        FShaderLibrary::Get()->Load("../Lumina/Engine/Resources/Shaders/InfiniteGrid.vert.spv",
      "../Lumina/Engine/Resources/Shaders/InfiniteGrid.frag.spv", "InfiniteGrid");

        FShaderLibrary::Get()->Load("../Lumina/Engine/Resources/Shaders/TAA.vert.spv",
        "../Lumina/Engine/Resources/Shaders/TAA.frag.spv", "TAA");
    }

    void FRenderer::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
        RenderAPI->InsertBarrier(BarrierInfo);
    }

    void FRenderer::BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TFastVector<uint32>& DynamicOffsets)
    {
        RenderAPI->BindSet(Set, Pipeline, SetIndex, DynamicOffsets);
    }

    void FRenderer::Submit(const RenderFunction& Functor)
    {
        InternalData.RenderFunctionList.push_back(Functor);
    }

    void FRenderer::BeginFrame()
    {
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
        RenderAPI->RenderMeshIndexed(Pipeline, VertexBuffer, IndexBuffer, Data);
    }

    void FRenderer::RenderVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
        RenderAPI->RenderVertices(Vertices, Instances, FirstVertex, FirstInstance);
    }

    void FRenderer::RenderStaticMeshWithMaterial(const TRefPtr<FPipeline>& Pipeline, const TAssetHandle<LStaticMesh>& StaticMesh, const TAssetHandle<Material>& Material)
    {
        RenderAPI->RenderStaticMeshWithMaterial(Pipeline, StaticMesh, Material);
    }

    void FRenderer::RenderStaticMesh(const TRefPtr<FPipeline>& Pipeline, std::shared_ptr<LStaticMesh> StaticMesh, uint32 InstanceCount)
    {
        RenderAPI->RenderStaticMesh(Pipeline, StaticMesh, InstanceCount);
    }

    void FRenderer::Render()
    {
        if(GetSwapchain()->IsSwapchainDirty())
        {
            InternalData.RenderFunctionList.clear();
            GetSwapchain()->RecreateSwapchain();
            return;
        }
        
        FRenderer::Submit([]
        {
            TRefPtr<FImage> Image = GetSwapchainImage();

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
        
        auto List = std::move(InternalData.RenderFunctionList);
        
        for(auto& func : List)
        {
            func();
        }
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