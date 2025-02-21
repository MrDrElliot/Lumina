#pragma once

#include <functional>
#include <glm/glm.hpp>
#include "Core/Functional/Function.h"
#include "Core/Templates/Forward.h"
#include "RenderResource.h"
#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Threading/Thread.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct FRenderPassBeginInfo;
    struct FPipelineBarrierInfo;
    enum class EShaderStage : uint8;
    class IRenderContext;
    class FImageSampler;
    class Material;
    class AStaticMesh;
    class IRenderAPI;
    class FSwapchain;
    class FBuffer;
    class FPipeline;
    class FImage;
    class FWindow;
    
    class FRenderer
    {
    public:
        
        using RenderFunction = TMoveOnlyFunction<void()>;
        
        struct RendererInternalData
        {
            uint32 NumDrawCalls = 0;
            uint32 NumVertices = 0;
            
            TQueue<RenderFunction>     RenderFunctionList;
            FRHIImageSampler           LinearSampler;
            FRHIImageSampler           NearestSampler;

            RendererInternalData(const RendererInternalData&) = delete;
            RendererInternalData& operator=(const RendererInternalData&) = delete;

            RendererInternalData() = default;
            ~RendererInternalData() = default;
            
        } static sInternalData;
        
        static void Init();
        static void Shutdown();

        template<typename LAMBDA>
        static void Submit(LAMBDA&& Lambda)
        {
            sInternalData.RenderFunctionList.push(TForward<LAMBDA>(Lambda));
        }
        
        static void BeginFrame();
        static void Present();
        static void ProcessCommands();
        
        static void BeginRender(const FRenderPassBeginInfo& Info);
        static void EndRender();
        
        static uint32 GetCurrentFrameIndex();

        static void WaitIdle();

        static FRHIImageSampler GetLinearSampler();
        static FRHIImageSampler GetNearestSampler();
        
        static void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo);
        static void BindSet(const FRHIDescriptorSet& Set, const FRHIPipeline& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets);
        static void BindPipeline(FRHIPipeline Pipeline);

        static IRenderContext* GetRenderContext();

        template<typename T>
        static T* GetRenderContext();
        
        static IRenderAPI* GetRenderAPI() { return RenderAPI; }

        template<typename T>
        static T* GetRenderAPI();
        
        static void CopyToSwapchain(FRHIImage ImageToCopy);
        static void ClearColor(const FRHIImage& Image, const glm::fvec4& Value);

        static void BindVertexBuffer(FRHIBuffer VertexBuffer);
        static void BindIndexBuffer(FRHIBuffer IndexBuffer);

        static void SetShaderParameter(const FName& ParameterName, void* Data, uint32 Size);
        
        static void PushConstants(FRHIPipeline Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data);
        static void DrawIndexed(uint32 IndexCount, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0);
        static void DrawVertices(uint32 Vertices, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0);

        
        static FRHICommandBuffer GetCommandBuffer();
        
    private:

        static IRenderAPI* RenderAPI;
    };

    
    //-------------------------------------------------------------------------------
    // Templates

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
