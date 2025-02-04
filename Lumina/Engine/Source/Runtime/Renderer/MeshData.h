#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina
{
    struct FVertex;

    struct FMeshAsset
    {

        FMeshAsset() = default;
        
        FMeshAsset(FMeshAsset&& Other) noexcept
            : Vertices(std::move(Other.Vertices))
            , Indices(std::move(Other.Indices))
        {}


        /** This data is much to large to accept copying. */
        FMeshAsset(const FMeshAsset&) = delete;
        FMeshAsset& operator=(const FMeshAsset&) = delete;
        
        
        friend FArchive& operator << (FArchive& Ar, FMeshAsset& data)
        {
            Ar << data.Vertices;
            Ar << data.Indices;

            return Ar;
        }

    public:
        
        TVector<FVertex> Vertices;
        TVector<uint32> Indices;
        
    };
}
