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

    }

    void FSceneRenderer::StartScene(const FScene* Scene)
    {
        FRenderPassBeginInfo PassInfo;
        PassInfo.Attachments = { GetPrimaryRenderTarget(), GetDepthAttachment() };
        PassInfo.bClearValue = true;
        PassInfo.ClearColor = FColor(0.0f, 0.0f, 0.0f, 1.0f);
        PassInfo.RenderAreaOffset = FIntVector2D(0, 0);
        PassInfo.RenderAreaExtent.Y = Windowing::GetPrimaryWindowHandle()->GetHeight();
        PassInfo.RenderAreaExtent.X = Windowing::GetPrimaryWindowHandle()->GetWidth();
        
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

        
        ForwardRenderPass(Scene);

        FRenderer::EndRender();
        
        
        // Copy render target to the swapchain image
        //-------------------------------------------------------------------
        
        FRHICommandBuffer CommandBuffer = FRenderer::GetCommandBuffer();
        FRHIImage CurrentRenderTarget = GetPrimaryRenderTarget();
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

    void FSceneRenderer::Shutdown()
    {
        FRenderer::WaitIdle();
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
    
    void FSceneRenderer::DrawPrimitives(const FScene* Scene)
    {
        
    }
    
    void FSceneRenderer::RenderGrid(const FScene* Scene)
    {
        // Begin rendering with the target and depth attachments
        FRHIImage CurrentRenderTarget = GetPrimaryRenderTarget();
        FRHIImage CurrentDepthAttachment = GetDepthAttachment();
        
    }

    void FSceneRenderer::RenderGeometry(const FScene* Scene)
    {
        FSceneRenderData RenderData;
        BuildSceneRenderData(&RenderData, Scene);

        uint32 CurrentFrameIndex = FRenderer::GetCurrentFrameIndex();
        FRHIDescriptorSet CurrentDescriptorSet = SceneGlobalDescriptorSets[CurrentFrameIndex];
        
        for (auto& MeshAssetHandle : RenderData.VisibleStaticMeshes)
        {
            const FMeshResource& MeshResource = MeshAssetHandle->GetMeshResource();

        } 
    }

    void FSceneRenderer::BuildSceneRenderData(FSceneRenderData* RenderData, const FScene* Scene)
    {
        Assert(RenderData != nullptr);
        
        auto RenderableView = Scene->GetConstEntityRegistry().view<FStaticMeshComponent>();

        for (auto Entity : RenderableView)
        {
            const FStaticMeshComponent& StaticMeshComponent = Scene->GetConstEntityRegistry().get<FStaticMeshComponent>(Entity);

            if (StaticMeshComponent.IsLoaded())
            {
                RenderData->VisibleStaticMeshes.push_back(StaticMeshComponent.StaticMesh);
            }
        }
    }
    
    /*void FSceneRenderer::GeometryPass(const FSceneUpdateContext& SceneContext)
    {
        FRHIImage CurrentRenderTarget = GetPrimaryRenderTarget();
        FRHIImage CurrentDepthTarget = GetDepthAttachment();
        
        TVector<FRHIImage> Attachments = {CurrentRenderTarget, CurrentDepthTarget, };

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
        TRefCountPtr<FDescriptorSet> CurrentDescriptorSet = SceneDescriptorSets[CurrentFrameIndex];
        
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
    }*/
    
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
        glm::uvec3 SwapchainSize = FRenderer::GetRenderContext()->GetSwapchain()->GetCurrentImage()->GetSpecification().Extent;
        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.x = SwapchainSize.x;
        ImageSpecs.Extent.y = SwapchainSize.y;
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
        DepthImageSpecs.Extent.x = SwapchainSize.x;
        DepthImageSpecs.Extent.y = SwapchainSize.y;
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
