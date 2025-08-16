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

        virtual bool Initialize() = 0;
        virtual void Deinitialize() = 0;

        virtual void WaitIdle() = 0;

        virtual void SetVSyncEnabled(bool bEnable) = 0;
        virtual bool IsVSyncEnabled() const = 0;

        
        //-------------------------------------------------------------------------------------


        virtual bool FrameStart(const FUpdateContext& UpdateContext, uint8 InCurrentFrameIndex) = 0;
        virtual bool FrameEnd(const FUpdateContext& UpdateContext) = 0;
        

        //-------------------------------------------------------------------------------------

        virtual FRHICommandListRef CreateCommandList(const FCommandListInfo& Info) = 0;
        virtual uint64 ExecuteCommandLists(ICommandList* const* CommandLists, uint32 NumCommandLists, ECommandQueue QueueType) = 0;
        NODISCARD virtual FRHICommandListRef GetCommandList(ECommandQueue Queue) = 0;

        //-------------------------------------------------------------------------------------

        NODISCARD virtual FRHIEventQueryRef CreateEventQuery() = 0;
        virtual void SetEventQuery(IEventQuery* Query, ECommandQueue Queue) = 0;
        virtual void ResetEventQuery(IEventQuery* Query) = 0;
        virtual void WaitEventQuery(IEventQuery* Query) = 0;
        virtual void PollEventQuery(IEventQuery* Query) = 0;
        

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

        NODISCARD virtual FRHIDescriptorTableRef CreateDescriptorTable(FRHIBindingLayout* InLayout) = 0;
        virtual void ResizeDescriptorTable(FRHIDescriptorTable* Table, uint32 NewSize, bool bKeepContents) = 0;
        virtual bool WriteDescriptorTable(FRHIDescriptorTable* Table, const FBindingSetItem& Binding) = 0;
        NODISCARD virtual FRHIInputLayoutRef CreateInputLayout(const FVertexAttributeDesc* AttributeDesc, uint32 Count) = 0;
        NODISCARD virtual FRHIBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc& Desc) = 0;
        NODISCARD virtual FRHIBindingLayoutRef CreateBindlessLayout(const FBindlessLayoutDesc& Desc) = 0;
        NODISCARD virtual FRHIBindingSetRef CreateBindingSet(const FBindingSetDesc& Desc, FRHIBindingLayout* InLayout) = 0;
        NODISCARD virtual FRHIComputePipelineRef CreateComputePipeline(const FComputePipelineDesc& Desc) = 0;
        NODISCARD virtual FRHIGraphicsPipelineRef CreateGraphicsPipeline(const FGraphicsPipelineDesc& Desc) = 0;
        
        
        //-------------------------------------------------------------------------------------

        virtual void OnShaderCompiled(FRHIShader* Shader) = 0;

        virtual void SetObjectName(IRHIResource* Resource, const char* Name, EAPIResourceType Type = EAPIResourceType::Default) = 0;

        
        NODISCARD virtual FRHIImageRef CreateImage(const FRHIImageDesc& ImageSpec) = 0;
        NODISCARD virtual FRHISamplerRef CreateSampler(const FSamplerDesc& SamplerDesc) = 0;

        // Front-end for executeCommandLists(..., 1) for compatibility and convenience
        uint64 ExecuteCommandList(ICommandList* CommandList, ECommandQueue ExecutionQueue = ECommandQueue::Graphics)
        {
            return ExecuteCommandLists(&CommandList, 1, ExecutionQueue);
        }

        //-------------------------------------------------------------------------------------

        
        virtual void FlushPendingDeletes() = 0;
    
    protected:
        
    };
}
