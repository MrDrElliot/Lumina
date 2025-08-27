#include "Mesh.h"

#include "Renderer/RenderContext.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "assets/assettypes/material/materialinstance.h"
#include "Core/Object/Cast.h"
#include "Renderer/RHIGlobals.h"


namespace Lumina
{
    void CMesh::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
        Ar << MeshResources;
    }

    void CMesh::Serialize(IStructuredArchive::FSlot Slot)
    {
        CObject::Serialize(Slot);
    }

    void CMesh::PostLoad()
    {
        SetMeshResource(MeshResources);
    }

    CMaterialInterface* CMesh::GetMaterialAtSlot(SIZE_T Slot) const
    {
        return Materials.empty() ? nullptr : Materials[Slot].Get();
    }

    void CMesh::SetMaterialAtSlot(SIZE_T Slot, CMaterialInterface* NewMaterial)
    {
        if (Materials.size() <= Slot)
        {
            Materials.push_back(NewMaterial);
        }
        else
        {
            Materials[Slot] = NewMaterial;
        }  
    }

    void CMesh::SetMeshResource(const FMeshResource& NewResource)
    {
        MeshResources = NewResource;
        GenerateBoundingBox();
        GenerateGPUBuffers();
    }

    bool CMesh::IsReadyForRender() const
    {
        LUMINA_PROFILE_SCOPE();

        if (HasAnyFlag(OF_NeedsLoad) || HasAllFlags(OF_MarkedGarbage))
        {
            return false;
        }

        for (CMaterialInterface* Material : Materials)
        {
            if (Material == nullptr)
            {
                return false;
            }

            if (Material->IsReadyForRender() == false)
            {
                return false;
            }
        }

        return !Materials.empty();
    }

    void CMesh::GenerateBoundingBox()
    {
        BoundingBox.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
        BoundingBox.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
        
        for (SIZE_T i = 0; i < MeshResources.Vertices.size(); ++i)
        {
            const FVertex& Vertex = MeshResources.Vertices[i];
            
            BoundingBox.Min.x = glm::min(Vertex.Position.x, BoundingBox.Min.x);
            BoundingBox.Min.y = glm::min(Vertex.Position.y, BoundingBox.Min.y);
            BoundingBox.Min.z = glm::min(Vertex.Position.z, BoundingBox.Min.z);
        
            BoundingBox.Max.x = glm::max(Vertex.Position.x, BoundingBox.Max.x);
            BoundingBox.Max.y = glm::max(Vertex.Position.y, BoundingBox.Max.y);
            BoundingBox.Max.z = glm::max(Vertex.Position.z, BoundingBox.Max.z);
        }
    }

    void CMesh::GenerateGPUBuffers()
    {
        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size = sizeof(FVertex) * MeshResources.Vertices.size();
        VertexBufferDesc.Stride = sizeof(FVertex);
        VertexBufferDesc.Usage.SetMultipleFlags(BUF_VertexBuffer);
        VertexBufferDesc.InitialState = EResourceStates::VertexBuffer;
        VertexBufferDesc.bKeepInitialState = true;
        VertexBufferDesc.DebugName = GetName().ToString() + "Vertex Buffer";
        MeshResources.VertexBuffer = GRenderContext->CreateBuffer(VertexBufferDesc);
        GRenderContext->SetObjectName(MeshResources.VertexBuffer, VertexBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

        

        FRHIBufferDesc IndexBufferDesc;
        IndexBufferDesc.Size = sizeof(uint32) * MeshResources.Indices.size();
        IndexBufferDesc.Stride = sizeof(uint32);
        IndexBufferDesc.Usage.SetMultipleFlags(BUF_IndexBuffer);
        IndexBufferDesc.InitialState = EResourceStates::IndexBuffer;
        IndexBufferDesc.bKeepInitialState = true;
        IndexBufferDesc.DebugName = GetName().ToString() + "Index Buffer";
        MeshResources.IndexBuffer = GRenderContext->CreateBuffer(IndexBufferDesc);
        GRenderContext->SetObjectName(MeshResources.IndexBuffer, IndexBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

        FRHICommandListRef CommandList = GRenderContext->CreateCommandList(FCommandListInfo::Graphics());

        CommandList->Open();
        CommandList->WriteBuffer(MeshResources.VertexBuffer, MeshResources.Vertices.data(), 0, MeshResources.Vertices.size() * sizeof(FVertex));
        CommandList->WriteBuffer(MeshResources.IndexBuffer, MeshResources.Indices.data(), 0, MeshResources.Indices.size() * sizeof(uint32));
        CommandList->Close();
        
        GRenderContext->ExecuteCommandList(CommandList, ECommandQueue::Graphics);
    }
}
