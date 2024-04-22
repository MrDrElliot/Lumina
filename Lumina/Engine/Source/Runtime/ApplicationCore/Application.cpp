#include "Application.h"
#include "ImGui/ImGuiLayer.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Buffer.h"
#include "Source/Runtime/Renderer/Image.h"
#include "Source/Runtime/Renderer/PipelineLibrary.h"
#include "Source/Runtime/Renderer/Renderer.h"
#include "Source/Runtime/Renderer/ShaderLibrary.h"
#include "Source/Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h"
#include "Source/Runtime/Scene/Scene.h"
#include "Windows/Window.h"


namespace Lumina
{
    FApplication* FApplication::Instance = nullptr;
    
    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        Instance = this;
        
    }

    FApplication::~FApplication()
    {
        
    }
     
    void FApplication::Run()
    {
        /* Application Initialization */
        OnInit();
        
        while(!ShouldExit())
        {
            if (!IsMinimized())
            {
                PreFrame();

                Window->OnUpdate(1.0f);
                
                PostFrame();
            }
        }

        /* Application Shutdown */
        OnShutdown();
    }

    int FApplication::Close()
    {
        return 0;
    }

    void FApplication::OnInit()
    {
        /* Initialize Logging */
        FLog::Init();

        LE_LOG_INFO("{0} Initializing", AppSpecs.Name);

        
        /* Create application window */
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = AppSpecs.Name;
        AppWindowSpecs.Width = AppSpecs.WindowWidth;
        AppWindowSpecs.Height = AppSpecs.WindowHeight;
        CreateApplicationWindow(AppWindowSpecs);
        
        FRenderConfig RenderConfig;
        RenderConfig.Window = Window.get();
        RenderConfig.FramesInFlight = 2;
        FRenderer::Init(RenderConfig);

        FImageSpecification ImageSpecs = FImageSpecification::Default();
        ImageSpecs.Extent.x = AppWindowSpecs.Width;
        ImageSpecs.Extent.y = AppWindowSpecs.Height;
        ImageSpecs.Usage = EImageUsage::RENDER_TARGET;
        ImageSpecs.Type = EImageType::TYPE_2D;
        ImageSpecs.Format = EImageFormat::RGBA32_SRGB;

        ColorImage = FImage::Create(ImageSpecs);
        
        DeviceBufferLayoutElement Element({"inPosition", EShaderDataType::FLOAT3});
        FDeviceBufferLayout Layout({Element});
        
        FPipelineSpecification PipelineSpecs = FPipelineSpecification::Default();
        PipelineSpecs.debug_name = "GraphicsPipeline";
        PipelineSpecs.shader = FShaderLibrary::GetShader("Mesh");
        PipelineSpecs.type = EPipelineType::GRAPHICS;
        PipelineSpecs.culling_mode = EPipelineCullingMode::NONE;
        PipelineSpecs.depth_test_enable = true;
        PipelineSpecs.output_attachments_formats = { EImageFormat::RGBA32_SRGB };
        PipelineSpecs.input_layout = Layout;
        std::shared_ptr<FPipeline> GraphicsPipeline = FPipeline::Create(PipelineSpecs);

                
        std::vector<float> vertices =
        {
            0.5,-0.5, 0,
            0.5, 0.5, 0,
           -0.5,-0.5, 0,
           -0.5, 0.5, 0,
       };
        

        std::vector<uint32_t> indices = {0, 1, 2, 2, 1, 3};

        FDeviceBufferSpecification Specs;
        Specs.BufferUsage = EDeviceBufferUsage::VERTEX_BUFFER;
        Specs.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        Specs.Heap = EDeviceBufferMemoryHeap::DEVICE;
        Specs.Size = sizeof(uint32_t) * vertices.size();
        VBO = FBuffer::Create(Specs, vertices.data(), vertices.size());
        VBO->UploadData(0, vertices.data(), vertices.size());

        FDeviceBufferSpecification IndexSpecs;
        IndexSpecs.BufferUsage = EDeviceBufferUsage::INDEX_BUFFER;
        IndexSpecs.MemoryUsage = EDeviceBufferMemoryUsage::NO_HOST_ACCESS;
        IndexSpecs.Heap = EDeviceBufferMemoryHeap::DEVICE;
        IndexSpecs.Size = sizeof(uint32_t) * indices.size();
        IBO = FBuffer::Create(IndexSpecs, indices.data(), indices.size());
        IBO->UploadData(0, indices.data(), indices.size());
        
    }

    void FApplication::OnShutdown()
    {
        bRunning = false;

        LE_LOG_WARN("Lumina Engine: Shutting Down");
        
        FRenderer::Shutdown();
        Window->Shutdown();
        glfwTerminate();
        

        FLog::Shutdown();
    }
    

    void FApplication::CreateApplicationWindow(const FWindowSpecs& InSpecs)
    {
        Window = FWindow::Create(InSpecs);
        Window->Init();
        Window->SetEventCallback(HZ_BIND_EVENT_FN(OnEvent));
        
    }

    void FApplication::PreFrame()
    {
        FRenderer::BeginFrame();
    }

    void FApplication::PostFrame()
    {

        FRenderer::BindPipeline(FPipelineLibrary::GetPipelineByTag("GraphicsPipeline"));

        
        glm::uvec3 RenderArea = {};
        RenderArea.x = AppSpecs.WindowWidth;
        RenderArea.y = AppSpecs.WindowHeight;
        FRenderer::BeginRender({ColorImage}, RenderArea, {0, 0},
            { 0.2f, 0.2f, 0.3f, 1.0 });

        
        FRenderer::RenderMeshIndexed(VBO, IBO);

        FRenderer::EndRender();
        
        FRenderer::Render();
        FRenderer::EndFrame();
        glfwPollEvents();
        
    }


    void FApplication::PushLayer(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PushLayer(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PushOverlay(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PushOverlay(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PopLayer(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PopLayer(InLayer);
        InLayer->OnDetach();
    }

    void FApplication::PopOverlay(std::shared_ptr<FLayer> InLayer)
    {
        LayerStack.PopOverlay(InLayer);
        InLayer->OnDetach();
    }

    void FApplication::InitImGuiLayer()
    {
        ImGuiLayer = FImGuiLayer::Create();
        PushOverlay(ImGuiLayer);
    }

    void FApplication::RenderImGui()
    {
        ImGuiLayer->Begin();

        for(auto Layer : LayerStack)
        {
            Layer->OnImGuiRender();
        }

        ImGuiLayer->End();
    }

    void FApplication::OnEvent(FEvent& Event)
    {
        for (auto Layer : LayerStack)
        {
            Layer->OnEvent(Event);
        }
    }

    bool FApplication::ShouldExit() const
    {
        return glfwWindowShouldClose(Window->GetWindow()) || !bRunning;
    }
}
