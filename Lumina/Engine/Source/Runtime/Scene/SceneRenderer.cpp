#include "SceneRenderer.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Core/Windows/Window.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Components/StaicMeshComponent.h"
#include "Entity/Entity.h"
#include "glm/gtc/type_ptr.hpp"
#include "Renderer/RHIIncl.h"
#include "Scene.h"
#include "SceneUpdateContext.h"
#include "Renderer/RenderPass.h"
#include "Subsystems/FCameraManager.h"

namespace Lumina
{
    FSceneRenderer* FSceneRenderer::Create()
    {
        return FMemory::New<FSceneRenderer>();
    }

    FSceneRenderer::FSceneRenderer()
        : SceneGlobalData()
        , SceneViewport(Windowing::GetPrimaryWindowHandle()->GetExtent())
    {
    }

    FSceneRenderer::~FSceneRenderer()
    {
    }

    void FSceneRenderer::Initialize()
    {
        CreateImages();
        InitPipelines();
        
        //InitBuffers();
        //InitDescriptorSets();
    }

    void FSceneRenderer::Deinitialize()
    {
        FRenderer::WaitIdle();
    }

    void FSceneRenderer::StartScene(const FScene* Scene)
    {
        FRenderPassBeginInfo PassInfo;
        PassInfo.Attachments = { GetPrimaryRenderTarget(), GetDepthAttachment() };
        PassInfo.bClearValue = true;
        PassInfo.ClearColor = FColor(0.0f, 0.0f, 0.0f, 1.0f);
        PassInfo.RenderAreaOffset = FIntVector2D(0, 0);
        PassInfo.RenderAreaExtent.Y = SceneViewport.GetSize().Y;
        PassInfo.RenderAreaExtent.X = SceneViewport.GetSize().X;
        
        FRenderer::BeginRender(PassInfo);

    }

    void FSceneRenderer::EndScene(const FScene* Scene)
    {
        PROFILE_SCOPE(StartFrame)

        //@TODO Fixme Move out of here and into somewhere that makes more sense
        if(FRenderer::GetRenderContext()->GetSwapchain()->WasSwapchainResizedThisFrame())
        {
            OnSwapchainResized();
        }


        // Setup scene global render state.
        //-------------------------------------------------------------------
        
        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        FCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<FCameraComponent>();
        
        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.Time =                  glfwGetTime();
        SceneGlobalData.DeltaTime =             Scene->GetSceneDeltaTime();

        SceneViewport.SetViewVolume(CameraComponent.GetViewVolume());
        
        
        ForwardRenderPass(Scene);
        
        Scene->GetPrimitiveDrawManager()->Draw(SceneGlobalData);

        FullScreenPass(Scene);
        
        FRenderer::EndRender();
        
        
        // Copy render target to the swapchain image if primary, and set layout.
        //-------------------------------------------------------------------


        FRHICommandBuffer CommandBuffer = FRenderer::GetCommandBuffer();
        FRHIImage CurrentRenderTarget = GetPrimaryRenderTarget();

        if (Scene->GetSceneType() == ESceneType::Game)
        {
        
            FRenderer::Submit([CurrentRenderTarget, CommandBuffer]
            {
                CurrentRenderTarget->SetLayout
                (
                    CommandBuffer,
                    EImageLayout::TRANSFER_SRC,
                    EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
                    EPipelineStage::TRANSFER,
                    EPipelineAccess::COLOR_ATTACHMENT_WRITE,
                    EPipelineAccess::TRANSFER_READ
                );

                FRenderer::GetRenderContext()->GetSwapchain()->GetCurrentImage()->SetLayout
                (
                    CommandBuffer,
                    EImageLayout::TRANSFER_DST,
                    EPipelineStage::TOP_OF_PIPE,
                    EPipelineStage::TRANSFER,
                    EPipelineAccess::NONE,
                    EPipelineAccess::TRANSFER_WRITE
                );
            
            });
        
            FRenderer::CopyToSwapchain(CurrentRenderTarget);
        }
        
        FRenderer::Submit([CurrentRenderTarget, CommandBuffer]
        {
            CurrentRenderTarget->SetLayout(CommandBuffer,
                EImageLayout::SHADER_READ_ONLY,
                EPipelineStage::TRANSFER,
                EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
                EPipelineAccess::TRANSFER_READ,
                EPipelineAccess::COLOR_ATTACHMENT_WRITE);
            
        });
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        RenderTargets.clear();
        DepthAttachments.clear();

        CreateImages();
        
    }

    void FSceneRenderer::ForwardRenderPass(const FScene* Scene)
    {
    }

    void FSceneRenderer::FullScreenPass(const FScene* Scene)
    {
        FPipelineSpec PipelineSpec = FPipelineSpec::Create()
        .SetPrimitiveTopology(EPipelineTopology::TRIANGLES)
        .SetPolygonFillMode(EPipelineFillMode::FILL)
        .SetCullingMode(EPipelineCullingMode::NONE)
        .SetEnableDepthTest(true)
        .SetAlphaBlendSrcFactor(EPipelineBlending::BLEND_FACTOR_ZERO)
        .SetAlphaBlendDstFactor(EPipelineBlending::BLEND_FACTOR_ONE)
        .SetLineWidth(1.0f)
        .SetShader("InfiniteGrid.glsl");
        
        FRHIPipeline Pipeline = FPipelineLibrary::Get()->GetOrCreatePipeline(PipelineSpec);
        
        FRenderer::BindPipeline(Pipeline);

        FSceneGlobalData Data = SceneGlobalData;
        FRenderer::SetShaderParameter("SceneUBO", &Data, sizeof(FSceneGlobalData));
        
        FRenderer::DrawVertices(6);
    }

    void FSceneRenderer::DrawPrimitives(const FScene* Scene)
    {
        
    }
    
    void FSceneRenderer::InitPipelines()
    {
        
        /*FMaterialPipelineSpecification InfiniteGridPipelineSpecs = FMaterialPipelineSpecification::Default();
        InfiniteGridPipelineSpecs
        .SetShader("InfiniteGrid")
        .SetPipelineType(EPipelineType::GRAPHICS)
        .SetCullingMode(EPipelineCullingMode::NONE)
        .SetEnableDepthTest(true)
        .SetAlphaBlendSrcFactor(EPipelineBlending::BLEND_FACTOR_ZERO)
        .SetAlphaBlendDstFactor(EPipelineBlending::BLEND_FACTOR_ONE)
        .SetRenderTargetFormats({EImageFormat::RGBA32_SRGB});
    
        SceneGlobalsPipeline = FPipeline::Create(InfiniteGridPipelineSpecs);
        SceneGlobalsPipeline->SetFriendlyName("Infinite Grid Pipeline");*/
    }


    void FSceneRenderer::InitBuffers()
    {
        //---------------------------------------------------------------
        // Scene Global Data (UBO)
        
        FDeviceBufferSpecification SceneGlobalDataSpec;
        SceneGlobalDataSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        SceneGlobalDataSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        SceneGlobalDataSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        SceneGlobalDataSpec.Size = sizeof(FSceneGlobalData);
        
        SceneGlobalUBO = FBuffer::Create(SceneGlobalDataSpec);
        SceneGlobalUBO->SetFriendlyName("Scene Global UBO");

        
        //---------------------------------------------------------------
        // Scene Light Data (SSBO)
        
        FDeviceBufferSpecification LightParamsSpec;
        LightParamsSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        LightParamsSpec.BufferUsage = EDeviceBufferUsage::STORAGE_BUFFER;
        LightParamsSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        LightParamsSpec.Size = sizeof(FSceneLightData);
        
        LightSSBO = FBuffer::Create(LightParamsSpec);
        LightSSBO->SetFriendlyName("Scene Light SSBO");


        
        //---------------------------------------------------------------
        // Model Data (SSBO)
        
        FDeviceBufferSpecification ModelParamsSpec;
        ModelParamsSpec.Heap =          EDeviceBufferMemoryHeap::DEVICE;
        ModelParamsSpec.BufferUsage =   EDeviceBufferUsage::STORAGE_BUFFER;
        ModelParamsSpec.MemoryUsage =   EDeviceBufferMemoryUsage::COHERENT_WRITE;
        ModelParamsSpec.Size = sizeof(FMeshModelData) * UINT16_MAX;
        
        ModelSSBO = FBuffer::Create(ModelParamsSpec);
        ModelSSBO->SetFriendlyName("Model SSBO");
        
    }

    
    void FSceneRenderer::InitDescriptorSets()
    {
        //GridMaterial = FMaterial::Create(InfiniteGridPipeline, FShaderLibrary::GetShader("Mesh"));
    }

    void FSceneRenderer::CreateImages()
    {
        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.X = SceneViewport.GetSize().X;
        ImageSpecs.Extent.Y = SceneViewport.GetSize().Y;
        ImageSpecs.Usage = EImageUsage::RENDER_TARGET;
        ImageSpecs.Type = EImageType::TYPE_2D;
        ImageSpecs.Format = EImageFormat::RGBA32_SRGB;
        ImageSpecs.SampleCount = EImageSampleCount::ONE;

        AssertMsg(RenderTargets.empty(), "Render Targets are not empty!");
        for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            FRHIImage Image = FImage::Create(ImageSpecs);
            Image->SetFriendlyName("Render Target: " + eastl::to_string(i));
            
            RenderTargets.push_back(std::move(Image));
        }

        FImageSpecification DepthImageSpecs = FImageSpecification::Default();
        DepthImageSpecs.Extent.X = SceneViewport.GetSize().X;
        DepthImageSpecs.Extent.Y = SceneViewport.GetSize().Y;
        DepthImageSpecs.Usage = EImageUsage::DEPTH_BUFFER;
        DepthImageSpecs.Type = EImageType::TYPE_2D;
        DepthImageSpecs.Format = EImageFormat::D32;
        DepthImageSpecs.SampleCount = EImageSampleCount::ONE;
        
        AssertMsg(DepthAttachments.empty(), "Render Targets are not empty!");
        for(int i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            FRHIImage Image = FImage::Create(DepthImageSpecs);
            Image->SetFriendlyName("Depth Image: " + eastl::to_string(i));
            
            DepthAttachments.push_back(std::move(Image));
        }

    }
    
}
