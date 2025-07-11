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
        TransferCommandList->WriteBuffer(VertexBuffer, MeshResource.Vertices.data(), 0, GetNumVertices() * sizeof(FVertex));
        
        // Index buffer
        FRHIBufferDesc IndexBufferDesc;
        IndexBufferDesc.Size = GetNumIndicies() * sizeof(uint32);
        IndexBufferDesc.Stride = sizeof(uint32);
        IndexBufferDesc.Usage.SetFlag(EBufferUsageFlags::IndexBuffer);

        IndexBuffer = RenderContext->CreateBuffer(IndexBufferDesc);
        
        TransferCommandList->WriteBuffer(IndexBuffer, MeshResource.Indices.data(), 0, GetNumIndicies() * sizeof(uint32));
        
        TransferCommandList->Close();
        RenderContext->ExecuteCommandList(TransferCommandList, 1, ECommandQueue::Transfer);

    }

    bool CMesh::IsReadyForRender() const
    {
        if (HasAnyFlag(OF_NeedsLoad) || HasAllFlags(OF_MarkedGarbage))
        {
            return false;
        }

        //@ TODO Temp until reflection is better supported.
        CMaterial* CastedMaterial = Cast<CMaterial>(Material);
        
        if (CastedMaterial == nullptr)
        {
            return false;
        }

        if (CastedMaterial->BindingLayout == nullptr
            || CastedMaterial->BindingSet == nullptr
            || CastedMaterial->VertexShader == nullptr
            || CastedMaterial->PixelShader == nullptr)
        {
            return false;
        }

        return true;
    }
}
