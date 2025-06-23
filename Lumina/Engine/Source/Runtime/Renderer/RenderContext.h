#pragma once
#include "CommandList.h"
#include "RenderResource.h"
#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Types/BitFlags.h"
#include "Core/UpdateContext.h"

namespace Lumina
{
    class IShaderCompiler;
}

namespace Lumina
{
    struct FCommandListInfo;
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

        virtual FRHICommandListRef CreateCommandList(const FCommandListInfo& Info = FCommandListInfo()) = 0;
        virtual void ExecuteCommandList(ICommandList* CommandLists, uint32 NumCommandLists = 1, ECommandQueue QueueType = ECommandQueue::Graphics) = 0;
        NODISCARD virtual FRHICommandListRef GetCommandList(ECommandQueue Queue = ECommandQueue::Graphics) = 0;


        //-------------------------------------------------------------------------------------

        
        NODISCARD virtual FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Description) = 0;
        virtual uint64 GetAlignedSizeForBuffer(uint64 Size, TBitFlags<EBufferUsageFlags> Usage) = 0;


        
        //-------------------------------------------------------------------------------------

        NODISCARD virtual FRHIViewportRef CreateViewport(const FIntVector2D& Size) = 0;

        
        //-------------------------------------------------------------------------------------
        
        NODISCARD virtual FRHIVertexShaderRef CreateVertexShader(const TVector<uint32>& ByteCode) = 0;
        NODISCARD virtual FRHIPixelShaderRef CreatePixelShader(const TVector<uint32>& ByteCode) = 0;
        NODISCARD virtual FRHIComputeShaderRef CreateComputeShader(const TVector<uint32>& ByteCode) = 0;

        NODISCARD virtual IShaderCompiler* GetShaderCompiler() const = 0;
        NODISCARD virtual FRHIShaderLibraryRef GetShaderLibrary() const = 0;

        virtual void CompileEngineShaders() = 0;

        
        //-------------------------------------------------------------------------------------

        NODISCARD virtual FRHIInputLayoutRef CreateInputLayout(const FVertexAttributeDesc* AttributeDesc, uint32 Count) = 0;
        NODISCARD virtual FRHIBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc& Desc) = 0;
        NODISCARD virtual FRHIBindingSetRef CreateBindingSet(const FBindingSetDesc& Desc, FRHIBindingLayout* InLayout) = 0;
        NODISCARD virtual FRHIComputePipelineRef CreateComputePipeline(const FComputePipelineDesc& Desc) = 0;
        NODISCARD virtual FRHIGraphicsPipelineRef CreateGraphicsPipeline(const FGraphicsPipelineDesc& Desc) = 0;
        
        
        //-------------------------------------------------------------------------------------


        
        NODISCARD virtual FRHIImageRef CreateImage(const FRHIImageDesc& ImageSpec) = 0;
        NODISCARD virtual FRHISamplerRef CreateSampler(const FSamplerDesc& SamplerDesc) = 0;
        

        //-------------------------------------------------------------------------------------

        
        virtual void FlushPendingDeletes() = 0;
        
    
    protected:
        
    };
}
