#include "SceneRenderer.h"

#include "ScenePrimitives.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/ApplicationCore/Windows/Window.h"
#include "Source/Runtime/Primitives/DefaultPrimitives.h"
#include "Source/Runtime/Renderer/Buffer.h"
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
        
        std::pair<std::vector<glm::vec3>, std::vector<glm::uint32>> Cube = Primitives::GenerateCube();

        FDeviceBufferSpecification VBOSPec;
        VBOSPec.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        VBOSPec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        VBOSPec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        VBOSPec.Size = sizeof(glm::vec3) * Cube.first.size();
        
        VBO = FBuffer::Create(VBOSPec, Cube.first.data(), sizeof(glm::vec3) * Cube.first.size());

        FDeviceBufferSpecification IBOSPec;
        IBOSPec.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IBOSPec.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IBOSPec.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IBOSPec.Size = sizeof(glm::uint32) * Cube.second.size();

        IBO = FBuffer::Create(IBOSPec, Cube.second.data(),sizeof(glm::uint32) * Cube.second.size());
        
    }

    FSceneRenderer::~FSceneRenderer()
    {
    }

    void FSceneRenderer::BeginScene(std::shared_ptr<FCamera> InCamera)
    {

    }

    void FSceneRenderer::EndScene()
    {
        std::shared_ptr<FPipeline> Pipeline = FPipelineLibrary::GetPipelineByTag("GraphicsPipeline");
        FRenderer::BindPipeline(Pipeline);

        
        glm::uvec3 RenderArea = {};
        RenderArea.x = FApplication::Get().GetWindow().GetWidth();
        RenderArea.y = FApplication::Get().GetWindow().GetHeight();
        FRenderer::BeginRender({ColorImage, DepthImage}, RenderArea, {0, 0},
            { 0.2f, 0.2f, 0.3f, 1.0 });

        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

        float fieldOfView = 45.0f;
        float aspectRatio = 800.0f / 600.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);

        // Combine the view and projection matrices
        glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

        // Prepare the data for push constants
        FMiscData CameraData;
        CameraData.Data = reinterpret_cast<glm::uint8*>(glm::value_ptr(viewProjectionMatrix));
        CameraData.Size = sizeof(glm::mat4);
        
        FRenderer::RenderMeshIndexed(Pipeline, VBO, IBO, CameraData);

        FRenderer::EndRender();

        FRenderer::Submit([&]
        {
            ColorImage->SetLayout(
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
        
        FRenderer::CopyToSwapchain(ColorImage);

        FRenderer::Submit([&]
        {
            ColorImage->SetLayout(
            FRenderer::GetCommandBuffer(),
            EImageLayout::COLOR_ATTACHMENT,
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
        
        DeviceBufferLayoutElement Float3( EShaderDataType::FLOAT3 );

        FDeviceBufferLayout Layout({Float3});
        
        FPipelineSpecification PipelineSpecs = FPipelineSpecification::Default();
        PipelineSpecs.debug_name = "GraphicsPipeline";
        PipelineSpecs.shader = FShaderLibrary::GetShader("Mesh");
        PipelineSpecs.type = EPipelineType::GRAPHICS;
        PipelineSpecs.culling_mode = EPipelineCullingMode::NONE;
        PipelineSpecs.depth_test_enable = true;
        PipelineSpecs.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        PipelineSpecs.input_layout = Layout;
        std::shared_ptr<FPipeline> GraphicsPipeline = FPipeline::Create(PipelineSpecs);
    }

    void FSceneRenderer::CreateImages()
    {
        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.x = FApplication::Get().GetWindow().GetWidth();
        ImageSpecs.Extent.y = FApplication::Get().GetWindow().GetHeight();
        ImageSpecs.Usage = EImageUsage::RENDER_TARGET;
        ImageSpecs.Type = EImageType::TYPE_2D;
        ImageSpecs.Format = EImageFormat::RGBA32_SRGB;

        ColorImage = FImage::Create(ImageSpecs);

        FImageSpecification DepthImageSpecs = FImageSpecification::Default();
        DepthImageSpecs.Extent.x = FApplication::Get().GetWindow().GetWidth();
        DepthImageSpecs.Extent.y = FApplication::Get().GetWindow().GetHeight();
        DepthImageSpecs.Usage = EImageUsage::DEPTH_BUFFER;
        DepthImageSpecs.Type = EImageType::TYPE_2D;
        DepthImageSpecs.Format = EImageFormat::D32;

        DepthImage = FImage::Create(DepthImageSpecs);
    }
}
