#pragma once
#include "RenderResource.h"
#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Core/Math/Math.h"
#include "Types/BitFlags.h"
#include "Core/UpdateContext.h"

namespace Lumina
{
    struct FGPUBarrier;
}

namespace Lumina
{
    
    class FCommandList
    {
    public:
        
        ECommandQueue CommandQueue;
        ECommandBufferUsage Type;
        ECommandBufferLevel Level;
    };

    class IRenderContext
    {
    public:
        virtual ~IRenderContext() = default;

        virtual void Initialize() = 0;
        virtual void Deinitialize() = 0;

        virtual void WaitIdle() = 0;

        virtual void SetVSyncEnabled(bool bEnable) = 0;
        virtual bool IsVSyncEnabled() const = 0;

        //-------------------------------------------------------------------------------------


        virtual void FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) = 0;
        virtual void FrameEnd(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) = 0;

        //-------------------------------------------------------------------------------------

        
        virtual FCommandList* BeginCommandList(ECommandBufferLevel Level = ECommandBufferLevel::Secondary, ECommandQueue CommandType = ECommandQueue::Graphics, ECommandBufferUsage Usage = ECommandBufferUsage::General) = 0;
        virtual void EndCommandList(FCommandList* CommandList) = 0;

        
        //-------------------------------------------------------------------------------------
        
        
        NODISCARD virtual FRHIBufferHandle CreateBuffer(TBitFlags<ERenderDeviceBufferUsage> UsageFlags, TBitFlags<ERenderDeviceBufferMemoryUsage> MemoryUsage, uint32 Size) = 0;
        virtual void UpdateBuffer(FRHIBufferHandle Buffer, void* Data, uint32 Size, uint32 Offset = 0) = 0;
        virtual void CopyBuffer(FRHIBufferHandle Source, FRHIBufferHandle Destination) = 0;
        virtual uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<ERenderDeviceBufferUsage> Usage) = 0;

        
        //-------------------------------------------------------------------------------------


        NODISCARD virtual FRHIShaderHandle CreateShader(const FString& Path) = 0;
        

        //-------------------------------------------------------------------------------------

        virtual void BindGraphicsPipeline(FCommandList* CommandList, FRHIGraphicsPipelineHandle Handle) = 0;
        virtual void BindComputePipeline(FCommandList* CommandList, FRHIComputePipelineHandle Handle) = 0;

        NODISCARD virtual FRHIGraphicsPipelineHandle CreateGraphicsPipeline(const FGraphicsPipelineSpec& PipelineSpec) = 0;
        NODISCARD virtual FRHIComputePipelineHandle CreateComputePipeline(const FComputePipelineSpec& PipelineSpec) = 0;
        
        
        //-------------------------------------------------------------------------------------

        
        NODISCARD virtual FRHIImageHandle AllocateImage() = 0;
        NODISCARD virtual FRHIImageHandle CreateTexture(const FImageSpecification& ImageSpec) = 0;
        NODISCARD virtual FRHIImageHandle CreateRenderTarget(const FIntVector2D& Extent) = 0;
        NODISCARD virtual FRHIImageHandle CreateDepthImage(const FImageSpecification& ImageSpec) = 0;

        virtual void Barrier(FGPUBarrier* Barriers, uint32 BarrierNum, FCommandList* CommandList) = 0;


        //-------------------------------------------------------------------------------------

        
        virtual void BeginRenderPass(FCommandList* CommandList, const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass(FCommandList* CommandList) = 0;

        virtual void ClearColor(FCommandList* CommandList, const FColor& Color) = 0;

        //-------------------------------------------------------------------------------------


        virtual void Draw(FCommandList* CommandList, uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) = 0;
        virtual void DrawIndexed(FCommandList* CommandList, uint32 IndexCount, uint32 InstanceCount = 1, uint32 FirstIndex = 1, int32 VertexOffset = 0, uint32 FirstInstance = 0) = 0;
        virtual void Dispatch(FCommandList* CommandList, uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) = 0;
    
    private:
        
    };
}
