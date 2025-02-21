#include "Renderer.h"
#include "Assets/AssetHandle.h"
#include "Renderer/RHIIncl.h"
#include "Paths/Paths.h"
#include "RHI/Vulkan/VulkanRenderAPI.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    IRenderAPI* FRenderer::RenderAPI = nullptr;
    FRenderer::RendererInternalData FRenderer::sInternalData;
    
    void FRenderer::Init()
    {
        LOG_TRACE("Renderer: Initializing");
        RenderAPI = IRenderAPI::Create();
        RenderAPI->Initialize();
        
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

        std::filesystem::path Path = Paths::GetEngineResourceDirectory();
        FString StringPath = Path.string().c_str();
        StringPath.append("/Shaders/");
        FShaderLibrary::Get()->LoadShadersInDirectory(StringPath);
        
    }
    
    void FRenderer::Shutdown()
    {
        LOG_TRACE("Renderer: Shutting Down");
        WaitIdle();

        while (!sInternalData.RenderFunctionList.empty())
        {
            sInternalData.RenderFunctionList.pop();
        }
        
        sInternalData.LinearSampler->Destroy();
        sInternalData.NearestSampler->Destroy();

        FShaderLibrary::Get()->Shutdown();
        FPipelineLibrary::Get()->Shutdown();
        
        delete RenderAPI;
    }

    FRHIImageSampler FRenderer::GetLinearSampler()
    {
        return sInternalData.LinearSampler;
    }

    FRHIImageSampler FRenderer::GetNearestSampler()
    {
        return sInternalData.NearestSampler;
    }

    void FRenderer::InsertBarrier(const FPipelineBarrierInfo& BarrierInfo)
    {
        RenderAPI->InsertBarrier(BarrierInfo);
    }

    void FRenderer::BindSet(const FRHIDescriptorSet& Set, const TRefCountPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets)
    {
        RenderAPI->BindSet(Set, Pipeline, SetIndex, DynamicOffsets);
    }

    void FRenderer::BeginFrame()
    {
        RenderAPI->BeginFrame();
    }
    
    void FRenderer::BeginRender(const FRenderPassBeginInfo& Info)
    {
        RenderAPI->BeginRender(Info);
    }

    void FRenderer::EndRender()
    {
        RenderAPI->EndRender();
    }

    void FRenderer::Present()
    {
        RenderAPI->Present();
    }
    
    void FRenderer::CopyToSwapchain(FRHIImage ImageToCopy)
    {
        RenderAPI->CopyToSwapchain(ImageToCopy);
    }

    void FRenderer::BindPipeline(FRHIPipeline Pipeline)
    {
        RenderAPI->BindPipeline(Pipeline);
        GetRenderContext()->SetCurrentPipeline(Pipeline);
    }

    IRenderContext* FRenderer::GetRenderContext()
    {
        return RenderAPI->GetRenderContext();
    }
    
    void FRenderer::DrawIndexed(uint32 IndexCount, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
        sInternalData.NumDrawCalls++;
        RenderAPI->DrawIndexed(IndexCount, Instances, FirstVertex, FirstInstance);
    }

    void FRenderer::DrawVertices(uint32 Vertices, uint32 Instances, uint32 FirstVertex, uint32 FirstInstance)
    {
        sInternalData.NumDrawCalls++;
        sInternalData.NumVertices += Vertices;
        RenderAPI->DrawVertices(Vertices, Instances, FirstVertex, FirstInstance);
    }

    void FRenderer::ProcessCommands()
    {
        sInternalData.NumDrawCalls = 0;
        sInternalData.NumVertices = 0;
        
        FRenderer::Submit([]
        {
            FRHIImage Image = GetRenderContext()->GetSwapchain()->GetCurrentImage();
        
            Image->SetLayout(
                FRenderer::GetCommandBuffer(),
                EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE,
                EPipelineAccess::TRANSFER_WRITE
            );
        });

        RenderAPI->EndCommandRecord();
        RenderAPI->ExecuteCurrentCommands();

        TQueue<RenderFunction> Commands;
        eastl::swap(Commands, sInternalData.RenderFunctionList);
        
        while (!Commands.empty())
        {
            auto& Func = Commands.front();
            Func();
            Commands.pop();
        }
    }

    uint32 FRenderer::GetFrameIndex()
    {
        return RenderAPI->GetRenderContext()->GetSwapchain()->GetFrameIndex();
    }

    void FRenderer::WaitIdle()
    {
        RenderAPI->WaitDevice();
    }

    void FRenderer::ClearColor(const FRHIImage& Image, const glm::fvec4& Value)
    {
        RenderAPI->ClearColor(Image, Value);
    }

    void FRenderer::BindVertexBuffer(FRHIBuffer VertexBuffer)
    {
        RenderAPI->BindVertexBuffer(VertexBuffer);
    }

    void FRenderer::BindIndexBuffer(FRHIBuffer IndexBuffer)
    {
        RenderAPI->BindIndexBuffer(IndexBuffer);
    }

    void FRenderer::SetShaderParameter(const FName& ParameterName, void* Data, uint32 Size)
    {
        RenderAPI->SetShaderParameter(ParameterName, Data, Size);
    }

    void FRenderer::PushConstants(FRHIPipeline Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data)
    {
        RenderAPI->PushConstants(Pipeline, ShaderStage, Offset, Size, Data);
    }
    
    FRHICommandBuffer FRenderer::GetCommandBuffer()
    {
        return GetRenderContext()->GetCommandBuffer();
    }
}
