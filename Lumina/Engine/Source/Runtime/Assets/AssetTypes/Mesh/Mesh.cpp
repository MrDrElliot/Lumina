#include "Mesh.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"


namespace Lumina
{
    void CMesh::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
        Ar << MeshResource;
    }

    void CMesh::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CMesh::PostLoad()
    {
        FRenderManager* RenderManager = GEngine->GetEngineSubsystem<FRenderManager>();
        IRenderContext* RenderContext = RenderManager->GetRenderContext();
        
        FRHICommandListRef TransferCommandList = RenderContext->CreateCommandList({ECommandQueue::Transfer});
        TransferCommandList->Open();
        
        // Vertex buffer
        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size = GetNumVertices() * sizeof(FVertex);
        VertexBufferDesc.Stride = sizeof(FVertex);
        VertexBufferDesc.Usage.SetFlag(EBufferUsageFlags::VertexBuffer);

        VertexBuffer = RenderContext->CreateBuffer(VertexBufferDesc);
        TransferCommandList->UploadToBuffer(VertexBuffer, MeshResource.Vertices.data(), 0, GetNumVertices() * sizeof(FVertex));
        
        // Index buffer
        FRHIBufferDesc IndexBufferDesc;
        IndexBufferDesc.Size = GetNumIndicies() * sizeof(uint32);
        IndexBufferDesc.Stride = sizeof(uint32);
        IndexBufferDesc.Usage.SetFlag(EBufferUsageFlags::IndexBuffer);

        IndexBuffer = RenderContext->CreateBuffer(IndexBufferDesc);
        
        TransferCommandList->UploadToBuffer(IndexBuffer, MeshResource.Indices.data(), 0, GetNumIndicies() * sizeof(uint32));
        
        TransferCommandList->Close();
        RenderContext->ExecuteCommandList(TransferCommandList, 1, ECommandQueue::Transfer);

    }
}
