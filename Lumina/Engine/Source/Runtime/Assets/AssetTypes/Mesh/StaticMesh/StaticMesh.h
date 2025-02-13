#pragma once

#include "Assets/AssetTypes/Mesh/Mesh.h"


namespace Lumina
{
    class AStaticMesh : public AMesh
    {
        DECLARE_ASSET("Static Mesh", StaticMesh, 1)
        
    public:
        
        explicit AStaticMesh(const FAssetPath& InPath)
            : AMesh(InPath)
        {}

        virtual ~AStaticMesh() override;

        void Serialize(FArchive& Ar) override
        {
            AMesh::Serialize(Ar);
        }
        
    };
}
