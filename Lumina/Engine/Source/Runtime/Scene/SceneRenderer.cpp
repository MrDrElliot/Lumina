#include "SceneRenderer.h"

#include "Camera.h"
#include "ScenePrimitives.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"
#include "Source/Runtime/Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Source/Runtime/Assets/Factories/TextureFactory/TextureFactory.h"
#include "Source/Runtime/Assets/StaticMesh/StaticMesh.h"
#include "Source/Runtime/Primitives/DefaultPrimitives.h"
#include "Source/Runtime/Renderer/Buffer.h"
#include "Source/Runtime/Renderer/DescriptorSet.h"
#include "Source/Runtime/Renderer/Pipeline.h"
#include "Source/Runtime/Renderer/PipelineLibrary.h"
#include "Source/Runtime/Renderer/Renderer.h"
#include "Source/Runtime/Renderer/ShaderLibrary.h"

namespace Lumina
{
    std::shared_ptr<FSceneRenderer> FSceneRenderer::Create()
    {
        return std::make_shared<FSceneRenderer>();
    }

    FSceneRenderer::FSceneRenderer()
    {
        CreateImages();
        InitPipelines();
        InitBuffers();
        InitDescriptorSets();
        
        RenderMeshes = FStaticMeshFactory::Import("../Lumina/Engine/Resources/Meshes/skull_downloadable.glb");
    
        
    }

    FSceneRenderer::~FSceneRenderer()
    {
      
    }
    void FSceneRenderer::Shutdown()
    {
        for (auto RT : RenderTargets)
        {
            RT->Destroy();
        }
        for (auto DA : DepthAttachments)
        {
            DA->Destroy();
        }
        RandomTexture->Destroy();
    }

    void FSceneRenderer::BeginScene(std::shared_ptr<FCamera> InCamera)
    {
        Camera = InCamera;
        InCamera->Update(FApplication::GetDeltaTime());
        
    }

    void FSceneRenderer::EndScene()
    {

        std::shared_ptr<FImage> CurrentRenderTarget = GetRenderTarget();
        std::shared_ptr<FImage> CurrentDepthAttachment = GetDepthAttachment();
        
        angle += rotationSpeed;
        
        std::shared_ptr<FPipeline> Pipeline = FPipelineLibrary::GetPipelineByTag("GraphicsPipeline");
        FRenderer::BindPipeline(Pipeline);
        

        glm::uvec3 RenderArea = {};
        RenderArea.x = FApplication::Get().GetWindow().GetWidth();
        RenderArea.y = FApplication::Get().GetWindow().GetHeight();
        FRenderer::BeginRender({CurrentRenderTarget, CurrentDepthAttachment}, RenderArea, {0, 0},{ 0.2f, 0.2f, 0.3f, 1.0 });
        
        glm::mat4 ViewProj = Camera->GetViewProjectionMatrix();
        CameraBuffer->UploadData(0, &ViewProj, sizeof(glm::mat4));

        TextureBuffer->UploadData(0, RandomTexture->GetSpecification().Pixels.data(), sizeof(glm::u8vec4) * RandomTexture->GetSpecification().Pixels.size());
        
        uint32_t CurrentIndex = FRenderer::GetCurrentFrameIndex();
        SceneDescriptorSets[CurrentIndex]->Write(0, 0, CameraBuffer, CameraBuffer->GetSpecification().Size, 0);
        SceneDescriptorSets[CurrentIndex]->Write(1, 0, TransformBuffer, TransformBuffer->GetSpecification().Size, 0);
        SceneDescriptorSets[CurrentIndex]->Write(2, 0, RandomTexture, LinearSampler);
        
        
        glm::vec3 position = glm::vec3(1.0f, 1.0f, 1.0f);

        float angleDegrees = cos(angle) * 360.0f;
        float angleRadians = glm::radians(angleDegrees);
        glm::vec3 rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 scaleFactors = glm::vec3(1.0f, 1.0f, 1.0f);

        // Build the transformation matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, angleRadians, rotationAxis);
        modelMatrix = glm::scale(modelMatrix, scaleFactors);
        
        // Calculate buffer index and upload the matrix
        TransformBuffer->UploadData(0, &modelMatrix, sizeof(glm::mat4));

        uint32_t* transIndex = new uint32_t(0);
        TransformData.Data = reinterpret_cast<const uint8_t*>(transIndex);
        TransformData.Size = sizeof(uint32_t);
        
        FRenderer::BindSet(SceneDescriptorSets[FRenderer::GetCurrentFrameIndex()], Pipeline, 0);
        FRenderer::RenderStaticMesh(Pipeline, RenderMeshes[0], TransformData);
        
        
        FRenderer::EndRender();
        

        FRenderer::Submit([CurrentRenderTarget]
        {
            CurrentRenderTarget->SetLayout(
                FRenderer::GetCommandBuffer(),
                EImageLayout::TRANSFER_SRC,
                EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
                EPipelineStage::TRANSFER,
                EPipelineAccess::COLOR_ATTACHMENT_WRITE,
                EPipelineAccess::TRANSFER_READ);

            FRenderer::GetSwapchainImage()->SetLayout(
                FRenderer::GetCommandBuffer(),
                EImageLayout::TRANSFER_DST,
                EPipelineStage::TOP_OF_PIPE,
                EPipelineStage::TRANSFER,
                EPipelineAccess::NONE,
                EPipelineAccess::TRANSFER_WRITE);
        });
        
        FRenderer::CopyToSwapchain(CurrentRenderTarget);

        FRenderer::Submit([CurrentRenderTarget]
        {
            CurrentRenderTarget->SetLayout(
            FRenderer::GetCommandBuffer(),
            EImageLayout::SHADER_READ_ONLY,
         EPipelineStage::TRANSFER,
         EPipelineStage::COLOR_ATTACHMENT_OUTPUT,
         EPipelineAccess::TRANSFER_READ,
         EPipelineAccess::COLOR_ATTACHMENT_WRITE);

            FRenderer::GetSwapchainImage()->SetLayout(
                FRenderer::GetCommandBuffer(),
                EImageLayout::PRESENT_SRC,
                EPipelineStage::TRANSFER,
                EPipelineStage::BOTTOM_OF_PIPE,
                EPipelineAccess::TRANSFER_WRITE,
                EPipelineAccess::NONE);
        });

    }

    void FSceneRenderer::InitPipelines()
    {
        
        DeviceBufferLayoutElement Pos( EShaderDataType::FLOAT3 );
        DeviceBufferLayoutElement Color( EShaderDataType::FLOAT4 );
        DeviceBufferLayoutElement UV(EShaderDataType::FLOAT2);

        FDeviceBufferLayout Layout({Pos, Color, UV});
        
        FPipelineSpecification PipelineSpecs = FPipelineSpecification::Default();
        PipelineSpecs.debug_name = "GraphicsPipeline";
        PipelineSpecs.shader = FShaderLibrary::GetShader("Mesh");
        PipelineSpecs.type = EPipelineType::GRAPHICS;
        PipelineSpecs.culling_mode = EPipelineCullingMode::BACK;
        PipelineSpecs.depth_test_enable = true;
        PipelineSpecs.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        PipelineSpecs.input_layout = Layout;
        std::shared_ptr<FPipeline> GraphicsPipeline = FPipeline::Create(PipelineSpecs);
    }

    void FSceneRenderer::InitBuffers()
    {
        FDeviceBufferSpecification Spec;
        Spec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        Spec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        Spec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        Spec.Size = sizeof FCameraData;
        CameraBuffer = FBuffer::Create(Spec);

        Spec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        Spec.BufferUsage = EDeviceBufferUsage::STORAGE_BUFFER;
        Spec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        Spec.Size = sizeof glm::mat4 * 10000;
        TransformBuffer = FBuffer::Create(Spec);

        Spec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        Spec.BufferUsage = EDeviceBufferUsage::UNIFORM_BUFFER;
        Spec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
        Spec.Size = sizeof(glm::u8vec4);
        TextureBuffer = FBuffer::Create(Spec);

    }

    void FSceneRenderer::InitDescriptorSets()
    {
        std::vector<FDescriptorBinding> Bindings;
        Bindings.push_back({ 0, EDescriptorBindingType::UNIFORM_BUFFER, 1, 0 });
        Bindings.push_back({ 1, EDescriptorBindingType::STORAGE_BUFFER, 1, 0 });
        Bindings.push_back({ 2, EDescriptorBindingType::SAMPLED_IMAGE, 1, 0 });

        FDescriptorSetSpecification GlobalSetSpec = {};
        GlobalSetSpec.Bindings = std::move(Bindings);

        for (int i = 0; i < FRenderer::GetConfig().FramesInFlight; i++)
        {
            auto Set = FDescriptorSet::Create(GlobalSetSpec);
            SceneDescriptorSets.push_back(Set);
        }
    }

    void FSceneRenderer::CreateImages()
    {
        FImageSamplerSpecification Nearest = {};
        Nearest.MinFilteringMode = ESamplerFilteringMode::LINEAR;
        Nearest.MagFilteringMode = ESamplerFilteringMode::NEAREST;
        Nearest.MipMapFilteringMode = ESamplerFilteringMode::LINEAR;
        Nearest.AddressMode = ESamplerAddressMode::REPEAT;
        Nearest.MinLOD = 0.0f;
        Nearest.MaxLOD = 1000.0f;
        Nearest.LODBias = 0.0f;
        Nearest.AnisotropicFilteringLevel = 1;

        NearestSampler = FImageSampler::Create(Nearest);
        
        // Initializing linear filtration sampler
        FImageSamplerSpecification Linear = {};
        Linear.MinFilteringMode = ESamplerFilteringMode::LINEAR;
        Linear.MagFilteringMode = ESamplerFilteringMode::LINEAR;
        Linear.MipMapFilteringMode = ESamplerFilteringMode::LINEAR;
        Linear.AddressMode = ESamplerAddressMode::REPEAT;
        Linear.MinLOD = 0.0f;
        Linear.MaxLOD = 1000.0f;
        Linear.LODBias = 0.0f;
        Linear.AnisotropicFilteringLevel = 1;

        LinearSampler = FImageSampler::Create(Linear);
        
        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.x = FApplication::Get().GetWindow().GetWidth();
        ImageSpecs.Extent.y = FApplication::Get().GetWindow().GetHeight();
        ImageSpecs.Usage = EImageUsage::RENDER_TARGET;
        ImageSpecs.Type = EImageType::TYPE_2D;
        ImageSpecs.Format = EImageFormat::RGBA32_SRGB;

        for (int i = 0; i < FRenderer::GetConfig().FramesInFlight; ++i)
        {
            RenderTargets.push_back(FImage::Create(ImageSpecs));
        }

        FImageSpecification DepthImageSpecs = FImageSpecification::Default();
        DepthImageSpecs.Extent.x = FApplication::Get().GetWindow().GetWidth();
        DepthImageSpecs.Extent.y = FApplication::Get().GetWindow().GetHeight();
        DepthImageSpecs.Usage = EImageUsage::DEPTH_BUFFER;
        DepthImageSpecs.Type = EImageType::TYPE_2D;
        DepthImageSpecs.Format = EImageFormat::D32;

        for(int i = 0; i < FRenderer::GetConfig().FramesInFlight; ++i)
        {
            DepthAttachments.push_back(FImage::Create(DepthImageSpecs));
        }

        FImageSpecification GBufferSpec = FImageSpecification::Default();
        GBufferSpec.Usage = EImageUsage::RENDER_TARGET;
        GBufferSpec.Format = EImageFormat::RGBA64_SFLOAT;
        GBufferSpec.Extent = FRenderer::GetSwapchainImage()->GetSpecification().Extent;

        /* Position */
        GBuffer.Positions = FImage::Create(GBufferSpec);

        /* Normals */
        GBuffer.Normals = FImage::Create(GBufferSpec);
        GBufferSpec.Format = EImageFormat::RGBA32_UNORM;

        /* Base Color */
        GBuffer.BaseColor = FImage::Create(GBufferSpec);

        /* Metallic, Roughness, Occlusion */
        GBuffer.MetallicRoughnessOcclusion = FImage::Create(GBufferSpec);





        std::vector<uint8_t> pixels;
   
        pixels = FTextureFactory::ImportFromSource("../Lumina/Engine/Resources/Meshes/defaultMat_baseColor.jpeg");

        
        // Configure the texture specifications
        FImageSpecification TextureSpec = FImageSpecification::Default();
        TextureSpec.Extent = {2048, 2048, 1};
        TextureSpec.Usage = EImageUsage::TEXTURE;
        TextureSpec.Format = EImageFormat::RGBA32_UNORM;
        TextureSpec.MipLevels = 1;
        TextureSpec.ArrayLayers = 1;
        TextureSpec.Pixels = pixels;

        // Create the texture
        RandomTexture = FImage::Create(TextureSpec);
        
        
    }
    
}
