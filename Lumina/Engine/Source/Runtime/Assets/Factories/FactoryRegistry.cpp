#include "FactoryRegistry.h"

#include "MeshFactory/StaticMeshFactory.h"
#include "TextureFactory/TextureFactory.h"

namespace Lumina
{
    FFactoryRegistry::FFactoryRegistry()
    {
        RegisterFactory<FTextureFactory, EAssetType::Texture>();
        RegisterFactory<FStaticMeshFactory, EAssetType::StaticMesh>();
    }

    FFactoryRegistry::~FFactoryRegistry()
    {
        for (auto KVP : FactoryMap)
        {
            delete KVP.second;    
        }

        FactoryMap.clear();
    }
}