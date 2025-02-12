#include "SceneRenderer.h"

#include "Scene.h"
#include "ScenePrimitives.h"
#include "SceneUpdateContext.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "glm/gtc/type_ptr.hpp"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Core/Math/Transform.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Core/Windows/Window.h"
#include "Entity/Entity.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Renderer/Buffer.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/Material.h"
#include "Renderer/Pipeline.h"
#include "Renderer/RenderContext.h"
#include "Renderer/Renderer.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/Swapchain.h"
#include "Subsystems/FCameraManager.h"

namespace Lumina
{
    FSceneRenderer* FSceneRenderer::Create(FScene* InScene)
    {
        return FMemory::New<FSceneRenderer>(InScene);
    }

    FSceneRenderer::FSceneRenderer(FScene* InScene)
        :CurrentScene(InScene)
    {

    }

    FSceneRenderer::~FSceneRenderer()
    {
      
    }

    void FSceneRenderer::Initialize(const FSubsystemManager& Manager)
    {
        CreateImages();
        InitPipelines();
        InitBuffers();
        InitDescriptorSets();

        // Create an identity matrix for the grid
        glm::mat4 GridMat = glm::mat4(1.0f);

        // Apply scaling to the grid (scale it by 5.0 in all directions)
        GridMat = glm::scale(GridMat, glm::vec3(5.0f));

        // Rotate the grid 90 degrees along the X-axis
        GridMat = glm::rotate(GridMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        
    }

    void FSceneRenderer::Deinitialize()
    {
    }

    void FSceneRenderer::Shutdown()
    {
        FRenderer::WaitIdle();
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        for (auto& RT : RenderTargets)
        {
            RT->Release();
        }
        RenderTargets.clear();
        
        for(auto& DT : DepthAttachments)
        {
            DT->Release();
        }
        DepthAttachments.clear();

        CreateImages();
        
    }

    void FSceneRenderer::RenderGrid()
    {
        // Begin rendering with the target and depth attachments
        TRefPtr<FImage> CurrentRenderTarget = GetPrimaryRenderTarget();
        TRefPtr<FImage> CurrentDepthAttachment = GetDepthAttachment();

        uint32 CurrentFrameIndex = FRenderer::GetCurrentFrameIndex();
        
        FRenderer::BeginRender({CurrentRenderTarget, CurrentDepthAttachment}, glm::vec4(CurrentScene->GetSceneSettings().BackgroundColor, 1.0f));

        FRenderer::BindPipeline(InfiniteGridPipeline);
        FRenderer::BindSet(GridDescriptorSets[CurrentFrameIndex], InfiniteGridPipeline, 0, {});
        
        GridDescriptorSets[CurrentFrameIndex]->Write(0, 0, CameraUBO, sizeof(FCameraData), 0);
        
        FRenderer::DrawVertices(6);

        FRenderer::EndRender();
    }

    void FSceneRenderer::StartFrame()
    {
        PROFILE_SCOPE(StartFrame)
        
        if(FRenderer::GetRenderContext()->GetSwapchain()->WasSwapchainResizedThisFrame())
        {
            OnSwapchainResized();
        }

        FCameraManager* CameraManager = CurrentScene->GetSceneSubsystem<FCameraManager>();
        FCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<FCameraComponent>();
        
        CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        CameraData.View =       CameraComponent.GetViewMatrix();
        CameraData.Projection = CameraComponent.GetProjectionMatrix();
        

        CameraUBO->UploadData(0, &CameraData, sizeof(FCameraData));

        
        if(CurrentScene->GetSceneSettings().bShowGrid)
        {
            RenderGrid();
        }
        
        //GeometryPass(SceneContext);
    }
    
    void FSceneRenderer::EndFrame()
    {
        PROFILE_SCOPE(EndFrame)

        TRefPtr<FCommandBuffer> CommandBuffer = FRenderer::GetCommandBuffer();
        TRefPtr<FImage> CurrentRenderTarget = GetPrimaryRenderTarget();
        FRenderer::Submit([CurrentRenderTarget, CommandBuffer]
        {
            CurrentRenderTarget->SetLayout(
                CommandBuffer,
                EImageLayout::TRANSFER_SRC,
                EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
                EPipelineStage::TRANSFER,
                EPipelineAccess::COLOR_ATTACHMENT_WRITE,
                EPipelineAccess::TRANSFER_READ);

            FRenderer::GetRenderContext()->GetSwapchain()->GetCurrentImage()->SetLayout(CommandBuffer,
                         EImageLayout::TRANSFER_DST,
                 EPipelineStage::TOP_OF_PIPE,
                 EPipelineStage::TRANSFER,
                EPipelineAccess::NONE,
                EPipelineAccess::TRANSFER_WRITE);
        });
        
        FRenderer::CopyToSwapchain(CurrentRenderTarget);

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

    void FSceneRenderer::GeometryPass(const FSceneUpdateContext& SceneContext)
    {
        TRefPtr<FImage> CurrentRenderTarget = GetPrimaryRenderTarget();
        TRefPtr<FImage> CurrentDepthTarget = GetDepthAttachment();
        
        TVector<TRefPtr<FImage>> Attachments = {CurrentRenderTarget, CurrentDepthTarget, };

        glm::vec4 ClearColor = glm::vec4(0.0f);
        // Begin rendering with the target and depth attachments
        if(CurrentScene->GetSceneSettings().bShowGrid)
        {
            FRenderer::BeginRender(Attachments, ClearColor);
        }
        else
        {
            FRenderer::BeginRender(Attachments, glm::vec4(CurrentScene->GetSceneSettings().BackgroundColor, 1.0f));
        }
        
        uint32 CurrentFrameIndex = FRenderer::GetCurrentFrameIndex();
        TRefPtr<FDescriptorSet> CurrentDescriptorSet = SceneDescriptorSets[CurrentFrameIndex];
        
        FRenderer::BindPipeline(GraphicsPipeline);
        FRenderer::BindSet(CurrentDescriptorSet, GraphicsPipeline, 1, {});

        CurrentDescriptorSet->Write(0, 0, CameraUBO, sizeof(FCameraData), 0);


        uint32 NumLights = 0;
        CurrentScene->ForEachComponent<FLightComponent>([&, this](uint32 Current, entt::entity& entity, FLightComponent& Component)
        {
            Entity Ent(entity, CurrentScene);
            FTransformComponent& TransformComponent = Ent.GetComponent<FTransformComponent>();
            SceneLightingData.Lights[Current].LightPosition = glm::vec4(TransformComponent.GetLocation(), 1.0f);
            SceneLightingData.Lights[Current].LightColor = Component.LightColor;
            NumLights++;
        });

        SceneLightingData.NumLights = NumLights;
        SceneUBO->UploadData(0, &SceneLightingData, sizeof(FSceneLightData));
        CurrentDescriptorSet->Write(1, 0, SceneUBO, sizeof(FSceneLightData), 0);    
        
        auto View = CurrentScene->GetEntityRegistry().view<FMeshComponent, FTransformComponent>();
        uint64 ComponentTotal = View.size_hint();

        bool bHasValidRender = false;
        if(ComponentTotal)
        {
            ModelData.clear();
            ModelData.reserve((int32)ComponentTotal);
            eastl::unordered_map<TSharedPtr<AMaterialInstance>, std::vector<TSharedPtr<AStaticMesh>>> MeshInstanceMap;

            CurrentScene->ForEachComponent<FMeshComponent>([&, this](uint32 Current, entt::entity& entity, FMeshComponent& Component)
            {
                if (Component.StaticMesh.IsLoaded())
                {

                    auto& Transform = View.get<FTransformComponent>(entity);

                    glm::mat4 Matrix = Transform.GetTransform().GetMatrix();
                    ModelData.emplace_back(std::move(Matrix));
                    
                    bHasValidRender = true;
                }
            });

            if (!bHasValidRender)
            {
                return;
            }
            
            ModelSBO->UploadData(0, ModelData.data(), ModelData.size() * sizeof(FModelData));
            CurrentDescriptorSet->Write(2, 0, ModelSBO, ModelData.size() * sizeof(FModelData), 0);

            TexturesData.clear();
            TexturesData.reserve(MeshInstanceMap.size());

            // A map to store each texture and its unique ID
            eastl::unordered_map<TSharedPtr<ATexture>, int32> TextureHash;

            // Unique texture counter
            int32 CurrentTextureID = 0;
            
            for (const auto& [MaterialInstance, Meshes] : MeshInstanceMap)
            {
                if (MaterialInstance != nullptr && MaterialInstance != nullptr)
                {
                    TexturesData.push_back(MaterialInstance->MaterialTextureIDs);
                }
                else
                {
                    // Skip invalid material instances
                    continue;
                }

                // Helper lambda to find or add a texture to the map
                auto GetOrAddTextureID = [&TextureHash, &CurrentTextureID](const TSharedPtr<ATexture>& Texture) -> int32
                {
                    if (!Texture)
                    {
                        return -1; // Invalid texture, -1 represents no image.
                    }
        
                    auto It = TextureHash.find(Texture);
                    if (It == TextureHash.end())
                    {
                        TextureHash[Texture] = CurrentTextureID++;
                        return TextureHash[Texture];
                    }
                    return It->second;
                };



            }

            
            // Write the texture IDs for the multiple textures found in TexturesData
            for (size_t i = 0; i < TexturesData.size(); ++i)
            {
                TestMaterial->Write(1, i, BaseColor, FRenderer::GetLinearSampler());
                TestMaterial->Write(1, i + 1, Normal, FRenderer::GetLinearSampler());
                TestMaterial->Write(1, i + 2, Metallic, FRenderer::GetLinearSampler());
                TestMaterial->Write(1, i + 3, Emissive, FRenderer::GetLinearSampler());
                TestMaterial->Write(1, i + 4, AmbientOcclusion, FRenderer::GetLinearSampler());   
            }

            MaterialUBO->UploadData(0, TexturesData.data(), TexturesData.size() * sizeof(FMaterialTexturesData));
            CurrentDescriptorSet->Write(3, 0, MaterialUBO, TexturesData.size() * sizeof(FMaterialTexturesData), 0);

            uint32 testIndex = 0;
            for (const auto& [MaterialInstance, Meshes] : MeshInstanceMap)
            {
                for (auto StaticMesh : Meshes)
                {
                    Data.ModelIndex = testIndex++;
                    Data.MaterialIndex = 0;
                    
                    FRenderer::PushConstants(GraphicsPipeline, EShaderStage::VERTEX, 0, sizeof(FTransientData), &Data);
                    FRenderer::PushConstants(GraphicsPipeline, EShaderStage::FRAGMENT, 16, sizeof(FMaterialAttributes), &Attributes);

                    
                    FRenderer::DrawIndexed(StaticMesh->GetVertexBuffer(), StaticMesh->GetIndexBuffer());
                    
                }
            }
        }
        
        // End the render
        FRenderer::EndRender();
    }
    
    void FSceneRenderer::InitPipelines()
    {
        DeviceBufferLayoutElement Pos               (EShaderDataType::FLOAT3);
        DeviceBufferLayoutElement Color             (EShaderDataType::FLOAT4);
        DeviceBufferLayoutElement NormalElement     (EShaderDataType::FLOAT3);
        DeviceBufferLayoutElement UV                (EShaderDataType::FLOAT2);

        FDeviceBufferLayout Layout({Pos, Color, NormalElement, UV});
    
        FPipelineSpecification PipelineSpecs = FPipelineSpecification::Default();
        PipelineSpecs.DebugName = "GraphicsPipeline";
        PipelineSpecs.shader = FShaderLibrary::GetShader("Mesh");
        PipelineSpecs.type = EPipelineType::GRAPHICS;
        PipelineSpecs.culling_mode = EPipelineCullingMode::BACK;
        PipelineSpecs.depth_test_enable = true;
        PipelineSpecs.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        PipelineSpecs.input_layout = Layout;
        
        GraphicsPipeline = FPipeline::Create(PipelineSpecs);
        GraphicsPipeline->SetFriendlyName("Graphics Pipeline");
        
        
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
        InfiniteGridPipeline->SetFriendlyName("Infinite Grid Pipeline");
    }


    void FSceneRenderer::InitBuffers()
    {
        
        FDeviceBufferSpecification CameraSpec;
        CameraSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        CameraSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        CameraSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        CameraSpec.Size = sizeof(FCameraData);
        CameraSpec.DebugName = "Camera Buffer";
        
        CameraUBO = FBuffer::Create(CameraSpec);
        CameraUBO->SetFriendlyName("Camera UBO");

        
        // SceneUBO contains lightPosition, cameraPosition
        FDeviceBufferSpecification LightParamsSpec;
        LightParamsSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        LightParamsSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        LightParamsSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        LightParamsSpec.Size = sizeof(FSceneLightData);
        LightParamsSpec.DebugName = "Scene Parameters SBO";
        
        SceneUBO = FBuffer::Create(LightParamsSpec);
        SceneUBO->SetFriendlyName("Scene Light UBO");

        
        
        // ModelUBO contains model.
        FDeviceBufferSpecification ModelParamsSpec;
        ModelParamsSpec.Heap =          EDeviceBufferMemoryHeap::DEVICE;
        ModelParamsSpec.BufferUsage =   EDeviceBufferUsage::STORAGE_BUFFER;
        ModelParamsSpec.MemoryUsage =   EDeviceBufferMemoryUsage::COHERENT_WRITE;
        ModelParamsSpec.Size = sizeof(glm::mat4) * UINT16_MAX;
        ModelParamsSpec.DebugName = "Model Storage Buffer";
        
        ModelSBO = FBuffer::Create(ModelParamsSpec);
        ModelSBO->SetFriendlyName("Model SBO");

        // ModelUBO contains model.
        FDeviceBufferSpecification MaterialBufferSpec;
        MaterialBufferSpec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        MaterialBufferSpec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        MaterialBufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        MaterialBufferSpec.Size = sizeof(FMaterialTexturesData) * UINT16_MAX;
        MaterialBufferSpec.DebugName = "Material Uniform Buffer";
        
        MaterialUBO = FBuffer::Create(MaterialBufferSpec);
        MaterialUBO->SetFriendlyName("Material UBO");
    }

    
    void FSceneRenderer::InitDescriptorSets()
    {

        // Initialize descriptor bindings
        TInlineVector<FDescriptorBinding, 4> Bindings;

        // Binding 0: CameraUniforms (Pos, View, Proj)
        Bindings.emplace_back( 0, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::VERTEX );

        // Binding 1: SceneParams (UBO containing light and scene data)
        Bindings.emplace_back( 1, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::FRAGMENT );

        // Binding 2: Containing all model data to be rendered for the scene.
        Bindings.emplace_back( 2, EDescriptorBindingType::STORAGE_BUFFER, 1, 0, EShaderStage::VERTEX );

        // Binding 3: MaterialTextureBuffer
        Bindings.emplace_back( 3, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::FRAGMENT );


        // Create descriptor set specification
        FDescriptorSetSpecification SceneSetSpec = {};
        SceneSetSpec.Bindings = std::move(Bindings);

        uint32 FramesInFlight = FRenderer::GetConfig().FramesInFlight;
        // Create descriptor sets for each frame
        for (uint32 i = 0; i < FramesInFlight; i++)
        {
            // Create the descriptor set using the specification
            auto Set = FDescriptorSet::Create(SceneSetSpec);
            Set->SetFriendlyName("Material: " + eastl::to_string(i));
            
            // Add the created set to the list of descriptor sets
            SceneDescriptorSets.push_back(std::move(Set));
        }
        
        TInlineVector<FDescriptorBinding, 4> GridBindings;

        // Binding 0: Grid (UBO view, proj matrices)
        GridBindings.emplace_back( 0, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0, EShaderStage::VERTEX );
        // Create descriptor set specification
        FDescriptorSetSpecification GridSpec = {};
        GridSpec.Bindings = std::move(GridBindings);

        for (uint32 i = 0; i < FramesInFlight; i++)
        {
            auto Set = FDescriptorSet::Create(GridSpec);
            Set->SetFriendlyName("Material: " + eastl::to_string(i));
            
            GridDescriptorSets.push_back(Set);
        }
    }

    void FSceneRenderer::CreateImages()
    {
        glm::uvec3 SwapchainSize = FRenderer::GetRenderContext()->GetSwapchain()->GetCurrentImage()->GetSpecification().Extent;
        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.x = SwapchainSize.x;
        ImageSpecs.Extent.y = SwapchainSize.y;
        ImageSpecs.Usage = EImageUsage::RENDER_TARGET;
        ImageSpecs.Type = EImageType::TYPE_2D;
        ImageSpecs.Format = EImageFormat::RGBA32_SRGB;
        ImageSpecs.SampleCount = EImageSampleCount::ONE;

        AssertMsg(RenderTargets.empty(), "Render Targets are not empty!");
        for (int i = 0; i < FRenderer::GetConfig().FramesInFlight; ++i)
        {
            TRefPtr<FImage> Image = FImage::Create(ImageSpecs);
            Image->SetFriendlyName("Render Target: " + eastl::to_string(i));
            
            RenderTargets.push_back(std::move(Image));
        }

        FImageSpecification DepthImageSpecs = FImageSpecification::Default();
        DepthImageSpecs.Extent.x = SwapchainSize.x;
        DepthImageSpecs.Extent.y = SwapchainSize.y;
        DepthImageSpecs.Usage = EImageUsage::DEPTH_BUFFER;
        DepthImageSpecs.Type = EImageType::TYPE_2D;
        DepthImageSpecs.Format = EImageFormat::D32;
        DepthImageSpecs.SampleCount = EImageSampleCount::ONE;
        
        AssertMsg(DepthAttachments.empty(), "Render Targets are not empty!");
        for(int i = 0; i < FRenderer::GetConfig().FramesInFlight; ++i)
        {
            TRefPtr<FImage> Image = FImage::Create(DepthImageSpecs);
            Image->SetFriendlyName("Depth Image: " + eastl::to_string(i));
            
            DepthAttachments.push_back(std::move(Image));
        }

    }
    
}
