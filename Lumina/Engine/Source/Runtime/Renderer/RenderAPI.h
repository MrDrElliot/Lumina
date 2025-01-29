#pragma once
#include <memory>
#include "Assets/AssetHandle.h"
#include "Image.h"
#include "PipelineBarrier.h"
#include "RenderTypes.h"
#include "Containers/Array.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    class FMaterial;
    class LMaterial;
    enum class EShaderStage : uint8;
    class Material;
    class LStaticMesh;
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
    
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginRender(const TVector<TRefPtr<FImage>>& Attachments, glm::fvec4 ClearColor) = 0;
        virtual void EndRender() = 0;
        virtual void WaitDevice() = 0;

        virtual FRenderConfig GetConfig() = 0;
        virtual TRefPtr<FSwapchain> GetSwapchain() = 0;
        virtual TRefPtr<FImage> GetSwapchainImage() = 0;
        virtual ERHIInterfaceType GetRHIInterfaceType() = 0;

        virtual void BindSet(const TRefPtr<FDescriptorSet>& Set, const TRefPtr<FPipeline>& Pipeline, uint8 SetIndex, const TVector<uint32>& DynamicOffsets) = 0;
        virtual void BindPipeline(TRefPtr<FPipeline> Pipeline) = 0;

        virtual void InsertBarrier(const FPipelineBarrierInfo& BarrierInfo) = 0;
        virtual void ClearColor(const TRefPtr<FImage>& Image, const glm::fvec4& Value) = 0;
        virtual void CopyToSwapchain(TRefPtr<FImage> ImageToCopy) = 0;

        virtual void PushConstants(EShaderStage ShaderStage, uint16 Offset, uint32 Size, const void* Data) = 0;
        
        virtual void RenderMeshTasks(TRefPtr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data) = 0;
        virtual void RenderMeshIndexed(TRefPtr<FPipeline> Pipeline, TRefPtr<FBuffer> VertexBuffer, TRefPtr<FBuffer> IndexBuffer, FMiscData Data) = 0;
        virtual void RenderVertices(uint32 Vertices, uint32 Instances = 1, uint32 FirstVertex = 0, uint32 FirstInstance = 0) = 0;
        virtual void RenderStaticMeshWithMaterial(const TRefPtr<FPipeline>& Pipeline, const TSharedPtr<LStaticMesh>& StaticMesh, const TRefPtr<FMaterial>& Material) = 0;
        virtual void RenderStaticMesh(const TRefPtr<FPipeline>& Pipeline, TSharedPtr<LStaticMesh> StaticMesh, uint32 InstanceCount) = 0;

        virtual TRefPtr<FCommandBuffer> GetCommandBuffer() = 0;
        virtual void BeginCommandRecord() = 0;
        virtual void EndCommandRecord() = 0;
        virtual void ExecuteCurrentCommands() = 0;
    };
}
