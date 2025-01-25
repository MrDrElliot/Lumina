#include "FactoryRegistry.h"

#include "MeshFactory/StaticMeshFactory.h"
#include "TextureFactory/TextureFactory.h"

Lumina::FFactoryRegistry::FFactoryRegistry()
{
    RegisterFactory<FTextureFactory, EAssetType::Texture>();
    RegisterFactory<FStaticMeshFactory, EAssetType::StaticMesh>();
}
