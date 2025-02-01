#pragma once

#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include "CommandBuffer.h"
#include "PipelineBarrier.h"
#include "RenderTypes.h"
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FMaterial;
    class FImageSampler;
    enum class EShaderStage : uint8;
    class Material;
    class AStaticMesh;
    class IRenderAPI;
    class FSwapchain;
    class FDescriptorSet;
    class FBuffer;
    class FPipeline;
    class FImage;
    class FWindow;
    class FRenderContext;

    struct FRenderConfig
    {
        uint32 FramesInFlight;
        FWindow* Window;
    };
    
    class FRenderer
    {
    public:
        
        using RenderFunction = std::move_only_function<void()>;
        
        struct RendererInternalData
        {
            uint32 NumDrawCalls = 0;
            uint32 NumVertices = 0;
            
            std::vector<RenderFunction>      RenderFunctionList;
            TRefPtr<FImageSampler>           LinearSampler;
            TRefPtr<FImageSampler>           NearestSampler;

            RendererInternalData(const RendererInternalData&) = delete;
            RendererInternalData& operator=(const RendererInternalData&) = delete;

            RendererInternalData() = default;
            ~RendererInternalData() = default;
            
        } static sInternalData;
        
        static void Init(const FRenderConfig& InConfig);
        static void Shutdown();

        /* Don't forget to capture function inputs if needed */
        static void Submit(RenderFunction&& Functor);

        
        static void BeginFrame();
        static void EndFrame();
        static void BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor = {0.0f, 0.0f, 0.0f, 0.0f});
        static void EndRender();
        static void Render();
        static void ProcessRenderQueue();

        static FRenderConfig GetConfig();
        static uint32 GetCurrentFrameIndex();

        static void WaitIdle();
        static void LoadShaderPack();

        static TRefPtr<FImageSampler> GetLinearSampler();
        static TRefPtr<FImageSampler> GetNearestSampler();
        
        static void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo);
        static void BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets);
        static void BindPipeline(TRefPtr<FPipeline> Pipeline);

        
        static TRefPtr<FSwapchain> GetSwapchain();
        static TRefPtr<FImage> GetSwapchainImage();
        
        static void CopyToSwapchain(TRefPtr<FImage> ImageToCopy);
        static void ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value);
        
        static void PushConstants(EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data);

        static void RenderMeshTasks(TRefPtr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data);
        static void RenderMeshIndexed(TRefPtr<FPipeline> Pipeline, TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer, FMiscData Data);
        static void RenderVertices(uint32 Vertices, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0);
        static void RenderStaticMeshWithMaterial(const TRefPtr<FPipeline>& Pipeline, const TSharedPtr<AStaticMesh>& StaticMesh, const TRefPtr<FMaterial>& Material);
        static void RenderStaticMesh(const TRefPtr<FPipeline>& Pipeline, TSharedPtr<AStaticMesh> StaticMesh, uint32 InstanceCount = 1);

        
        static TRefPtr<FCommandBuffer> GetCommandBuffer();
        
    private:

        static IRenderAPI* RenderAPI;
    };
    
}
