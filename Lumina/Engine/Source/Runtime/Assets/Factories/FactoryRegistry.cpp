#include "FactoryRegistry.h"

#include "MaterialFactory/MaterialFactory.h"
#include "MeshFactory/StaticMeshFactory.h"
#include "TextureFactory/TextureFactory.h"

namespace Lumina
{
    FFactoryRegistry::FFactoryRegistry()
    {
        RegisterFactory<FMaterialFactory, EAssetType::Material>();
        RegisterFactory<FTextureFactory, EAssetType::Texture>();
        RegisterFactory<FStaticMeshFactory, EAssetType::StaticMesh>();
    }

    FFactoryRegistry::~FFactoryRegistry()
    {
        for (auto KVP : FactoryMap)
        {
            FMemory::Delete(KVP.second);
        }

        FactoryMap.clear();
        Factories.clear();
    }
}