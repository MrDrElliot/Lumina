#pragma once

#include "Renderer/Buffer.h"
#include "Memory/RefCounted.h"
#include "Assets/AssetTypes/Mesh/Mesh.h"
#include "Containers/Array.h"
#include "Renderer/RenderTypes.h"


namespace Lumina
{
    class FBuffer;
    
    
    class AStaticMesh : public FMesh
    {
    public:
        
        explicit AStaticMesh(const FAssetPath& InPath)
            : FMesh(InPath)
        {}

        DECLARE_ASSET("Static Mesh", StaticMesh, 1)

        virtual ~AStaticMesh() override;

    private:

    };
}
