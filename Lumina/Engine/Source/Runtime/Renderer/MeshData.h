#pragma once

#include "Containers/Array.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    struct FVertex;

    struct FGeometrySurface final
    {
        FName ID;
        SIZE_T StartIndex = 0;
        uint32 IndexCount = 0;
        int64 MaterialIndex = -1;

        friend FArchive& operator << (FArchive& Ar, FGeometrySurface& Data)
        {
            Ar << Data.ID;
            Ar << Data.StartIndex;
            Ar << Data.IndexCount;
            Ar << Data.MaterialIndex;

            return Ar;
        }
        
    };

    struct FMeshResource final
    {
        FName                       Name;
        TVector<FVertex>            Vertices;
        TVector<uint32>             Indices;
        TVector<FGeometrySurface>   GeometrySurfaces;

        SIZE_T GetNumSurfaces() const { return GeometrySurfaces.size(); }

        friend FArchive& operator << (FArchive& Ar, FMeshResource& Data)
        {
            Ar << Data.Name;
            Ar << Data.Vertices;
            Ar << Data.Indices;
            Ar << Data.GeometrySurfaces;

            return Ar;
        }
    };
}
