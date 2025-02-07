#pragma once

#include <functional>
#include <glm/glm.hpp>
#include "CommandBuffer.h"
#include "PipelineBarrier.h"
#include "Core/Functional/Function.h"
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class IRenderContext;
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

    struct FRenderConfig
    {
        uint8 FramesInFlight;
        FWindow* Window;
    };
    
    class FRenderer
    {
    public:
        
        using RenderFunction = TMoveOnlyFunction<void()>;
        
        struct RendererInternalData
        {
            uint32 NumDrawCalls = 0;
            uint32 NumVertices = 0;
            
            TQueue<RenderFunction>           RenderFunctionList;
            TRefPtr<FImageSampler>           LinearSampler;
            TRefPtr<FImageSampler>           NearestSampler;

            RendererInternalData(const RendererInternalData&) = delete;
            RendererInternalData& operator=(const RendererInternalData&) = delete;

            RendererInternalData() = default;
            ~RendererInternalData() = default;
            
        } static sInternalData;
        
        static void Init(const FRenderConfig& InConfig);
        static void Shutdown();

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

        static IRenderContext* GetRenderContext();

        template<typename T>
        static T* GetRenderContext();
        
        static IRenderAPI* GetRenderAPI() { return RenderAPI; }

        template<typename T>
        static T* GetRenderAPI();
        
        static void CopyToSwapchain(TRefPtr<FImage> ImageToCopy);
        static void ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value);

        
        static void PushConstants(TRefPtr<FPipeline> Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data);
        static void DrawIndexed(TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer);
        static void DrawVertices(uint32 Vertices, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0);

        
        static TRefPtr<FCommandBuffer> GetCommandBuffer();
        
    private:

        static IRenderAPI* RenderAPI;
    };


    template <typename T>
    T* FRenderer::GetRenderContext()
    {
        static_assert(std::is_base_of_v<IRenderContext, T>, "T must inherit from IRenderContext");
        return static_cast<T*>(GetRenderContext());
    }

    template <typename T>
    T* FRenderer::GetRenderAPI()
    {
        static_assert(std::is_base_of_v<IRenderAPI, T>, "T must inherit from IRenderAPI");
        return static_cast<T*>(RenderAPI);
    }
}
