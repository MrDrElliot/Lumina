#pragma once

#include "Containers/Array.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    struct FVertex;

    struct FMeshResource final
    {
        friend class CMesh;
        friend class CStaticMeshFactory;
        
        TVector<FVertex> Vertices;
        TVector<uint32>  Indices;


        friend FArchive& operator << (FArchive& Ar, FMeshResource& Data)
        {
            Ar << Data.Vertices;
            Ar << Data.Indices;

            return Ar;
        }
    };
}
