#pragma once

#include "PipelineBarrier.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "Containers/Array.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    enum class EShaderStage : uint8;
    class FMaterial;
    class Material;
    class AStaticMesh;
    class FSwapchain;
    class FBuffer;
    class FPipeline;
    class FCommandBuffer;
    struct FRenderConfig;

    enum class ERHIInterfaceType : uint8
    {
        Vulkan,
        OpenGL,
        DX12,
    };
    
    class IRenderAPI
    {
    public:
        
        virtual ~IRenderAPI() = default;

        static IRenderAPI* Create();

        FORCEINLINE IRenderContext* GetRenderContext() const { return RenderContext; }

        virtual void Initialize() = 0;
        
        template<typename T>
        T* GetRenderContext();
    
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginRender(const FRenderPassBeginInfo& Info) = 0;
        virtual void EndRender() = 0;
        virtual void WaitDevice() = 0;

        virtual ERHIInterfaceType GetRHIInterfaceType() = 0;

        virtual void BindSet(const FRHIDescriptorSet& Set, const FRHIPipeline& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets) = 0;
        virtual void BindPipeline(FRHIPipeline Pipeline) = 0;

        virtual void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo) = 0;
        virtual void ClearColor(const FRHIImage& Image, const glm::fvec4& Value) = 0;
        virtual void CopyToSwapchain(FRHIImage ImageToCopy) = 0;

        virtual void PushConstants(FRHIPipeline Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data) = 0;

        virtual void BindVertexBuffer(FRHIBuffer VertexBuffer) = 0;
        virtual void BindIndexBuffer(FRHIBuffer IndexBuffer) = 0;
        
        virtual void DrawIndexed(uint32 IndexCount, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0) = 0;
        virtual void DrawVertices(uint32 Vertices, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0) = 0;

        virtual void BeginCommandRecord() = 0;
        virtual void EndCommandRecord() = 0;
        virtual void ExecuteCurrentCommands() = 0;
    
    protected:

        IRenderContext*                         RenderContext = nullptr;

    };

    //--------------------------------------------------------------
    // Templates
    //--------------------------------------------------------------

    template <typename T>
    T* IRenderAPI::GetRenderContext()
    {
        static_assert(std::is_base_of_v<IRenderContext, T>, "T must be a subclass of IRenderContext");
        return static_cast<T*>(RenderContext);
    }

}
