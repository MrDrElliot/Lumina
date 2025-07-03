#include "Mesh.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"


namespace Lumina
{
    void CMesh::Serialize(FArchive& Ar)
    {
        Ar << MeshResource;
    }

    void CMesh::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CMesh::UpdateStreamableResource()
    {
        FRenderManager* RenderManager = GEngine->GetEngineSubsystem<FRenderManager>();
        IRenderContext* RenderContext = RenderManager->GetRenderContext();
        ICommandList* CommandList = RenderContext->GetCommandList();
        
        
        // Vertex buffer
        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size = GetNumVertices() * sizeof(FVertex);
        VertexBufferDesc.Stride = sizeof(FVertex);
        VertexBufferDesc.Usage.SetFlag(EBufferUsageFlags::VertexBuffer);

        VertexBuffer = RenderContext->CreateBuffer(VertexBufferDesc);
        CommandList->UploadToBuffer(VertexBuffer, MeshResource.Vertices.data(), 0, GetNumVertices() * sizeof(FVertex));
        
        // Index buffer
        FRHIBufferDesc IndexBufferDesc;
        IndexBufferDesc.Size = GetNumIndicies() * sizeof(uint32);
        IndexBufferDesc.Stride = sizeof(uint32);
        IndexBufferDesc.Usage.SetFlag(EBufferUsageFlags::IndexBuffer);

        IndexBuffer = RenderContext->CreateBuffer(IndexBufferDesc);
        CommandList->UploadToBuffer(IndexBuffer, MeshResource.Indices.data(), 0, GetNumIndicies() * sizeof(uint32));


    }
}
