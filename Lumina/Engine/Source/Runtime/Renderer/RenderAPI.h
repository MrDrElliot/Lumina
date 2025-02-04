#pragma once

#include "Assets/AssetHandle.h"
#include "Image.h"
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
    class FDescriptorSet;
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

        static IRenderAPI* Create(const FRenderConfig& InConfig);

        FORCEINLINE IRenderContext* GetRenderContext() const { return RenderContext; }

        virtual void Initialize(const FRenderConfig& InConfig) = 0;
        
        template<typename T>
        T* GetRenderContext();
    
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor) = 0;
        virtual void EndRender() = 0;
        virtual void WaitDevice() = 0;

        virtual FRenderConfig GetConfig() { return Config; }
        virtual ERHIInterfaceType GetRHIInterfaceType() = 0;

        virtual void BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets) = 0;
        virtual void BindPipeline(TRefPtr<FPipeline> Pipeline) = 0;

        virtual void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo) = 0;
        virtual void ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value) = 0;
        virtual void CopyToSwapchain(TRefPtr<FImage> ImageToCopy) = 0;

        virtual void PushConstants(TRefPtr<FPipeline> Pipeline, EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data) = 0;
        
        virtual void DrawIndexed(TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer) = 0;
        virtual void DrawVertices(uint32 Vertices, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0) = 0;

        virtual void BeginCommandRecord() = 0;
        virtual void EndCommandRecord() = 0;
        virtual void ExecuteCurrentCommands() = 0;
    
    protected:

        IRenderContext*                         RenderContext = nullptr;
        FRenderConfig                           Config = {};

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
