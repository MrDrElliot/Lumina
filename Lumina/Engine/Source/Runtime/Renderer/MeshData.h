#pragma once

#include "Containers/Array.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    struct FVertex;

    class FMeshResource final
    {
    public:

        friend class AMesh;
        
        FMeshResource() = default;
        
        FMeshResource(FMeshResource&& Other) noexcept
            : Vertices(std::move(Other.Vertices))
            , Indices(std::move(Other.Indices))
        {}


        /** This data is much to large to accept copying. */
        FMeshResource(const FMeshResource&) = delete;
        FMeshResource& operator=(const FMeshResource&) = delete;

        
        FORCEINLINE const TRefPtr<FBuffer>& GetVertexBuffer()   { return VBO; }
        FORCEINLINE const TRefPtr<FBuffer>& GetIndexBuffer()    { return IBO; }
        
        friend FArchive& operator << (FArchive& Ar, FMeshResource& Data)
        {
            Ar << Data.Vertices;
            Ar << Data.Indices;

            return Ar;
        }

    private:
        
        TVector<FVertex>          Vertices;
        TVector<uint32>           Indices;

        // -------- Transient Data --------
        
        TRefPtr<FBuffer>          VBO;
        TRefPtr<FBuffer>          IBO;
        
    };
}
