#include "MaterialFactory.h"

namespace Lumina
{
    std::shared_ptr<LAsset> FMaterialFactory::CreateNew(const FAssetMetadata& Metadata, FArchive& Archive)
    {
        return std::shared_ptr<LAsset>();
    }
}
