#include "SceneRenderer.h"

#include "Camera.h"
#include "Scene.h"
#include "ScenePrimitives.h"
#include "Core/Application.h"
#include "glm/gtc/type_ptr.hpp"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Components/MeshComponent.h"
#include "Core/LuminaMacros.h"
#include "Core/Windows/Window.h"
#include "Entity/Entity.h"
#include "Renderer/Buffer.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/Material.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Renderer.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/Swapchain.h"

namespace Lumina
{
    std::shared_ptr<FSceneRenderer> FSceneRenderer::Create(LScene* InScene)
    {
        return std::make_shared<FSceneRenderer>(InScene);
    }

    FSceneRenderer::FSceneRenderer(LScene* InScene)
        :CurrentScene(InScene)
    {
        CreateImages();
        InitPipelines();
        InitBuffers();
        InitDescriptorSets();
        
    }

    FSceneRenderer::~FSceneRenderer()
    {
      
    }
    void FSceneRenderer::Shutdown()
    {
        FRenderer::WaitIdle();
        
        Camera->Destroy();

        SceneUBO->Destroy();
        ModelSBO->Destroy();
        Camera->Destroy();
        GridUBO->Destroy();
        
        for (auto RT : RenderTargets)
        {
            RT->Destroy();
        }
        RenderTargets.clear();
        
        for (auto DA : DepthAttachments)
        {
            DA->Destroy();
        }
        DepthAttachments.clear();

        for(auto Desc : SceneDescriptorSets)
        {
            Desc->Destroy();
        }
        SceneDescriptorSets.clear();
        
        for(auto Desc : GridDescriptorSets)
        {
            Desc->Destroy();
        }
        GridDescriptorSets.clear();
        
        for(auto Desc : TAADescriptorSets)
        {
            Desc->Destroy();
        }
        TAADescriptorSets.clear();

        GraphicsPipeline->Destroy();
        InfiniteGridPipeline->Destroy();
        TAAPipeline->Destroy();
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        for (auto& RT : RenderTargets)
        {
            RT->Destroy();
        }
        RenderTargets.clear();
        
        for(auto& DT : DepthAttachments)
        {
            DT->Destroy();
        }
        DepthAttachments.clear();

        CreateImages();
        
    }

    void FSceneRenderer::RenderGrid()
    {
        // Begin rendering with the target and depth attachments
        TRefPtr<FImage> CurrentRenderTarget = GetRenderTarget();
        TRefPtr<FImage> CurrentDepthAttachment = GetDepthAttachment();

        uint32 CurrentFrameIndex = FRenderer::GetCurrentFrameIndex();
        
        glm::vec3 CameraPos = Camera->GetPosition();
        // Create an identity matrix for the grid
        glm::mat4 GridMat = glm::mat4(1.0f);

        // Apply scaling to the grid (scale it by 5.0 in all directions)
        GridMat = glm::scale(GridMat, glm::vec3(5.0f));

        // Rotate the grid 90 degrees along the X-axis
        GridMat = glm::rotate(GridMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        
        GridData.View = Camera->GetViewMatrix();
        GridData.Projection = Camera->GetProjectionMatrixNoReverse();
        GridData.Position = Camera->GetPosition();
        
        FRenderer::BeginRender({CurrentRenderTarget, CurrentDepthAttachment}, glm::vec4(CurrentScene->GetSceneSettings().BackgroundColor, 1.0f));

        FRenderer::BindPipeline(InfiniteGridPipeline);
        FRenderer::BindSet(GridDescriptorSets[CurrentFrameIndex], InfiniteGridPipeline, 0, {});
        
        GridUBO->UploadData(0, &GridData, sizeof(FGridData));
        GridDescriptorSets[CurrentFrameIndex]->Write(0, 0, GridUBO, sizeof(FGridData), 0);
        
        FRenderer::RenderVertices(6);

        FRenderer::EndRender();
    }

    void FSceneRenderer::BeginScene(std::shared_ptr<FCamera> InCamera)
    {
        Camera = InCamera;
                
        InCamera->Update(FApplication::GetDeltaTime());
    }

    void FSceneRenderer::EndScene()
    {
        if(FRenderer::GetSwapchain()->WasSwapchainResizedThisFrame())
        {
            OnSwapchainResized();
        }
        
        uint32 CurrentFrameIndex = FRenderer::GetCurrentFrameIndex();
        TRefPtr<FDescriptorSet> CurrentDescriptorSet = SceneDescriptorSets[CurrentFrameIndex];
        
        TRefPtr<FImage> CurrentRenderTarget = GetRenderTarget();
        TRefPtr<FImage> CurrentDepthAttachment = GetDepthAttachment();

        if(CurrentScene->GetSceneSettings().bShowGrid)
        {
            RenderGrid();
        }
        
        GeometryPass({CurrentRenderTarget, CurrentDepthAttachment});

        TAAPass();
        
        TRefPtr<FCommandBuffer> CommandBuffer = FRenderer::GetCommandBuffer();
        FRenderer::Submit([CurrentRenderTarget, CommandBuffer]
        {
            CurrentRenderTarget->SetLayout(
                CommandBuffer,
                EImageLayout::TRANSFER_SRC,
                EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
                EPipelineStage::TRANSFER,
                EPipelineAccess::COLOR_ATTACHMENT_WRITE,
                EPipelineAccess::TRANSFER_READ);

            FRenderer::GetSwapchainImage()->SetLayout(CommandBuffer, EImageLayout::TRANSFER_DST, EPipelineStage::TOP_OF_PIPE,
                EPipelineStage::TRANSFER,
                EPipelineAccess::NONE,
                EPipelineAccess::TRANSFER_WRITE);
        });
        
        FRenderer::CopyToSwapchain(CurrentRenderTarget);

        FRenderer::Submit([CurrentRenderTarget, CommandBuffer]
        {
            CurrentRenderTarget->SetLayout(CommandBuffer, EImageLayout::SHADER_READ_ONLY, EPipelineStage::TRANSFER,
                EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
                EPipelineAccess::TRANSFER_READ,
                EPipelineAccess::COLOR_ATTACHMENT_WRITE);
            
        });

    }

    void FSceneRenderer::GeometryPass(const TFastVector<TRefPtr<FImage>>& Attachments)
    {
        // Begin rendering with the target and depth attachments
        if(CurrentScene->GetSceneSettings().bShowGrid)
        {
            FRenderer::BeginRender(Attachments);
        }
        else
        {
            FRenderer::BeginRender(Attachments, glm::vec4(CurrentScene->GetSceneSettings().BackgroundColor, 1.0f));
        }
        
        uint32 CurrentFrameIndex = FRenderer::GetCurrentFrameIndex();
        TRefPtr<FDescriptorSet> CurrentDescriptorSet = SceneDescriptorSets[CurrentFrameIndex];
        
        FRenderer::BindPipeline(GraphicsPipeline);
        FRenderer::BindSet(CurrentDescriptorSet, GraphicsPipeline, 1, {});
        
        CameraData.View = Camera->GetViewMatrix();
        CameraData.Projection = Camera->GetProjectionMatrix();
        SceneLightingData.cameraPosition = glm::vec4(Camera->GetPosition(), 1.0f);
        
        CameraUBO->UploadData(0, &CameraData, sizeof(FCameraData));
        SceneUBO->UploadData(0, &SceneLightingData, sizeof(FLightData));
        
        CurrentDescriptorSet->Write(0, 0, CameraUBO, sizeof(FCameraData), 0);
        CurrentDescriptorSet->Write(1, 0, SceneUBO, sizeof(FLightData), 0);

        ModelData.clear();
        auto View = CurrentScene->GetEntityRegistry().view<MeshComponent, TransformComponent>();
        uint64 ComponentTotal = View.size_hint();
        ModelData.reserve((uint32)ComponentTotal);
        
        for(auto entity : View)
        {
            auto& Transform = View.get<TransformComponent>(entity);
            glm::mat4 Matrix = Transform.Transform.GetMatrix();
    
            ModelData.emplace_back(std::move(Matrix));
        }
        
        if(ComponentTotal)
        {
            ModelSBO->UploadData(0, ModelData.data(), ComponentTotal * sizeof(glm::mat4));
            CurrentDescriptorSet->Write(1, 0, SceneUBO, sizeof(FLightData), 0);
            CurrentDescriptorSet->Write(2, 0, ModelSBO, ComponentTotal * sizeof(glm::mat4), 0);
            
            CurrentScene->ForEachComponent<MeshComponent>([this](uint32 Total, uint32 Current, entt::entity& entity, MeshComponent& Component)
            {
                Entity Ent(entity, CurrentScene);
                Component.Material->Bind(GraphicsPipeline);
                FRenderer::PushConstants(EShaderStage::VERTEX,   0, sizeof(uint32), &Current);
                FRenderer::PushConstants(EShaderStage::FRAGMENT, 16, sizeof(FMaterialAttributes), &Component.Material->GetMaterialAttributes());
                FRenderer::RenderStaticMeshWithMaterial(GraphicsPipeline, Component.StaticMesh, Component.Material);
            });
        }
        
        // End the render
        FRenderer::EndRender();
    }

    void FSceneRenderer::TAAPass()
    {

    }

    void FSceneRenderer::InitPipelines()
    {
        DeviceBufferLayoutElement Pos(EShaderDataType::FLOAT3);
        DeviceBufferLayoutElement Color(EShaderDataType::FLOAT4);
        DeviceBufferLayoutElement UV(EShaderDataType::FLOAT2);
        DeviceBufferLayoutElement Normal(EShaderDataType::FLOAT3);

        FDeviceBufferLayout Layout({Pos, Color, Normal, UV});
    
        FPipelineSpecification PipelineSpecs = FPipelineSpecification::Default();
        PipelineSpecs.DebugName = "GraphicsPipeline";
        PipelineSpecs.shader = FShaderLibrary::GetShader("Mesh");
        PipelineSpecs.type = EPipelineType::GRAPHICS;
        PipelineSpecs.culling_mode = EPipelineCullingMode::BACK;
        PipelineSpecs.depth_test_enable = true;
        PipelineSpecs.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        PipelineSpecs.input_layout = Layout;
        
        GraphicsPipeline = FPipeline::Create(PipelineSpecs);
        
        
        FPipelineSpecification InfiniteGridPipelineSpecs = FPipelineSpecification::Default();
        InfiniteGridPipelineSpecs.DebugName = "InfiniteGrid Pipeline";
        InfiniteGridPipelineSpecs.shader = FShaderLibrary::GetShader("InfiniteGrid");
        InfiniteGridPipelineSpecs.type = EPipelineType::GRAPHICS;
        InfiniteGridPipelineSpecs.culling_mode = EPipelineCullingMode::NONE;
        InfiniteGridPipelineSpecs.depth_test_enable = false;
        InfiniteGridPipelineSpecs.SrcAlphaBlendFactor = EPipelineBlending::BLEND_FACTOR_ZERO;
        InfiniteGridPipelineSpecs.DstAlphaBlendFactor = EPipelineBlending::BLEND_FACTOR_ONE;
        InfiniteGridPipelineSpecs.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        InfiniteGridPipelineSpecs.input_layout = {};
    
        InfiniteGridPipeline = FPipeline::Create(InfiniteGridPipelineSpecs);

        FPipelineSpecification TAAPipelineSpec = FPipelineSpecification::Default();
        TAAPipelineSpec.DebugName = "TAA Pipeline";
        TAAPipelineSpec.shader = FShaderLibrary::GetShader("TAA");
        TAAPipelineSpec.type = EPipelineType::GRAPHICS;
        TAAPipelineSpec.culling_mode = EPipelineCullingMode::NONE;
        TAAPipelineSpec.depth_test_enable = false;
        TAAPipelineSpec.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        TAAPipelineSpec.input_layout = {};

        TAAPipeline = FPipeline::Create(TAAPipelineSpec);
    }


    void FSceneRenderer::InitBuffers()
    {

        FDeviceBufferSpecification GridSpec;
        GridSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        GridSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        GridSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        GridSpec.Size = sizeof(FGridData);
        GridSpec.DebugName = "GridUBO";
        
        GridUBO = FBuffer::Create(GridSpec);
        
        
        FDeviceBufferSpecification CameraSpec;
        CameraSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        CameraSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        CameraSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        CameraSpec.Size = sizeof(FCameraData);
        CameraSpec.DebugName = "Camera Buffer";
        
        CameraUBO = FBuffer::Create(CameraSpec);
        
        
        // SceneUBO contains lightPosition, cameraPosition
        FDeviceBufferSpecification LightParamsSpec;
        LightParamsSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        LightParamsSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        LightParamsSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        LightParamsSpec.Size = sizeof(FLightData);
        LightParamsSpec.DebugName = "Scene Parameters UBO";
        
        SceneUBO = FBuffer::Create(LightParamsSpec);
        
        
        // ModelUBO contains model.
        FDeviceBufferSpecification ModelParamsSpec;
        ModelParamsSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        ModelParamsSpec.BufferUsage = EDeviceBufferUsage::STORAGE_BUFFER;
        ModelParamsSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        ModelParamsSpec.Size = sizeof(glm::mat4) * UINT16_MAX;
        ModelParamsSpec.DebugName = "Model Storage Buffer";
        
        ModelSBO = FBuffer::Create(ModelParamsSpec);
    }

    
    void FSceneRenderer::InitDescriptorSets()
    {

        // Initialize descriptor bindings
        std::vector<FDescriptorBinding> Bindings;

        // Binding 0: MaterialUniforms (UBO view, proj matrices)
        Bindings.emplace_back( 0, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::VERTEX );

        // Binding 1: SceneParams (UBO containing light and scene data)
        Bindings.emplace_back( 1, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::FRAGMENT );

        // Binding 2: Containing all model data to be rendered for the scene.
        Bindings.emplace_back( 2, EDescriptorBindingType::STORAGE_BUFFER, 1, 0, EShaderStage::VERTEX );


        // Create descriptor set specification
        FDescriptorSetSpecification SceneSetSpec = {};
        SceneSetSpec.Bindings = std::move(Bindings);

        uint32 FramesInFlight = FRenderer::GetConfig().FramesInFlight;
        // Create descriptor sets for each frame
        for (uint32 i = 0; i < FramesInFlight; i++)
        {
            // Create the descriptor set using the specification
            auto Set = FDescriptorSet::Create(SceneSetSpec);

            // Add the created set to the list of descriptor sets
            SceneDescriptorSets.push_back(std::move(Set));
        }
        
        std::vector<FDescriptorBinding> GridBindings;

        // Binding 0: Grid (UBO view, proj matrices)
        GridBindings.emplace_back( 0, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::VERTEX );
        // Create descriptor set specification
        FDescriptorSetSpecification GridSpec = {};
        GridSpec.Bindings = std::move(GridBindings);

        for (uint32 i = 0; i < FramesInFlight; i++)
        {
            GridDescriptorSets.push_back(FDescriptorSet::Create(GridSpec));
        }

        
        std::vector<FDescriptorBinding> TAABindings;
        TAABindings.emplace_back(0, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT); // Current frame color
        TAABindings.emplace_back(1, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT); // Previous frame color
        TAABindings.emplace_back(2, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0, EShaderStage::FRAGMENT); // Motion vectors
        
        // Create descriptor set specification
        FDescriptorSetSpecification TAASpec = {};
        TAASpec.Bindings = std::move(TAABindings);

        for (uint32 i = 0; i < FramesInFlight; i++)
        {
            GridDescriptorSets.push_back(FDescriptorSet::Create(TAASpec));
        }
    }

    void FSceneRenderer::CreateImages()
    {
        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.x = FRenderer::GetSwapchainImage()->GetSpecification().Extent.x;
        ImageSpecs.Extent.y = FRenderer::GetSwapchainImage()->GetSpecification().Extent.y;
        ImageSpecs.Usage = EImageUsage::RENDER_TARGET;
        ImageSpecs.Type = EImageType::TYPE_2D;
        ImageSpecs.Format = EImageFormat::RGBA32_SRGB;
        ImageSpecs.SampleCount = EImageSampleCount::ONE;

        AssertMsg(RenderTargets.empty(), "Render Targets are not empty!");
        for (int i = 0; i < FRenderer::GetConfig().FramesInFlight; ++i)
        {
            RenderTargets.push_back(FImage::Create(ImageSpecs));
        }

        FImageSpecification DepthImageSpecs = FImageSpecification::Default();
        DepthImageSpecs.Extent.x = FRenderer::GetSwapchainImage()->GetSpecification().Extent.x;
        DepthImageSpecs.Extent.y = FRenderer::GetSwapchainImage()->GetSpecification().Extent.y;
        DepthImageSpecs.Usage = EImageUsage::DEPTH_BUFFER;
        DepthImageSpecs.Type = EImageType::TYPE_2D;
        DepthImageSpecs.Format = EImageFormat::D32;
        DepthImageSpecs.SampleCount = EImageSampleCount::ONE;

        AssertMsg(DepthAttachments.empty(), "Render Targets are not empty!");
        for(int i = 0; i < FRenderer::GetConfig().FramesInFlight; ++i)
        {
            DepthAttachments.push_back(FImage::Create(DepthImageSpecs));
        }

    }
    
}
