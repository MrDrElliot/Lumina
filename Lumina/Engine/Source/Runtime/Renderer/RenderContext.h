#pragma once
#include "RenderResource.h"
#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Core/Math/Math.h"
#include "Types/BitFlags.h"
#include "Core/UpdateContext.h"

namespace Lumina
{
    class ICommandList;
    struct FGPUBarrier;
}

namespace Lumina
{
    
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
        virtual void FrameEnd(const FUpdateContext& UpdateContext) = 0;
        

        //-------------------------------------------------------------------------------------

        
        NODISCARD virtual ICommandList* AllocateCommandList(ECommandBufferLevel Level = ECommandBufferLevel::Secondary, ECommandQueue CommandType = ECommandQueue::Graphics, ECommandBufferUsage Usage = ECommandBufferUsage::General) = 0;

        
        //-------------------------------------------------------------------------------------
        
        
        NODISCARD virtual FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Description) = 0;
        NODISCARD virtual void UploadToBuffer(ICommandList* CommandList, FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size) = 0;
        virtual void CopyBuffer(ICommandList* CommandList, FRHIBuffer* Source, FRHIBuffer* Destination) = 0;
        virtual uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage) = 0;

        
        //-------------------------------------------------------------------------------------
        

        NODISCARD virtual FRHIVertexShaderRef CreateVertexShader(const TVector<const uint32>& ByteCode) = 0;
        NODISCARD virtual FRHIPixelShaderRef CreatePixelShader(const TVector<const uint32>& ByteCode) = 0;
        NODISCARD virtual FRHIComputeShaderRef CreateComputeShader(const TVector<const uint32>& ByteCode) = 0;
        
        //-------------------------------------------------------------------------------------


        
        NODISCARD virtual FRHIImageRef CreateImage(const FRHIImageDesc& ImageSpec) = 0;

        

        //-------------------------------------------------------------------------------------

        
        virtual void BeginRenderPass(ICommandList* CommandList, const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass(ICommandList* CommandList) = 0;

        virtual void ClearColor(ICommandList* CommandList, const FColor& Color) = 0;

        
        //-------------------------------------------------------------------------------------


        virtual void Draw(ICommandList* CommandList, uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) = 0;
        virtual void DrawIndexed(ICommandList* CommandList, uint32 IndexCount, uint32 InstanceCount = 1, uint32 FirstIndex = 1, int32 VertexOffset = 0, uint32 FirstInstance = 0) = 0;
        virtual void Dispatch(ICommandList* CommandList, uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) = 0;

        virtual void FlushPendingDeletes() = 0;
    
    private:
        
    };
}
