#pragma once

#include "RenderResource.h"
#include "Core/Math/Color.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FQueue;
}

namespace Lumina
{
    struct FRenderPassBeginInfo;
    class IRenderContext;
}

namespace Lumina
{
    enum class ECommandQueue : uint8;


    struct FCommandListInfo
    {
        
        /** Type of command queue that this list is to be executed on */
        ECommandQueue       CommandQueue = ECommandQueue::Graphics;
        
    };
    
    class ICommandList : public IRHIResource
    {
    public:

        RENDER_RESOURCE(RRT_CommandList)

        ICommandList() = default;
        virtual ~ICommandList() override = default;

        virtual void Open() = 0;
        virtual void Close() = 0;
        virtual void Executed(FQueue* Queue) = 0;
        
        NODISCARD virtual void UploadToBuffer(FRHIBuffer* Buffer, void* Data, uint32 Offset, uint32 Size) = 0;
        virtual void CopyBuffer(FRHIBuffer* Source, FRHIBuffer* Destination) = 0;
        
        virtual void BeginRenderPass(const FRenderPassBeginInfo& PassInfo) = 0;
        virtual void EndRenderPass() = 0;
        virtual void ClearColor(const FColor& Color) = 0;
        virtual void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstVertex, uint32 FirstInstance) = 0;
        virtual void DrawIndexed(uint32 IndexCount, uint32 InstanceCount = 1, uint32 FirstIndex = 1, int32 VertexOffset = 0, uint32 FirstInstance = 0) = 0;
        virtual void Dispatch(uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ) = 0;

        virtual const FCommandListInfo& GetCommandListInfo() const = 0;
        
        
    protected:
        
    };
}
