#include "MaterialFactory.h"

namespace Lumina
{
    TSharedPtr<LAsset> FMaterialFactory::CreateNew(const FAssetMetadata& Metadata, FArchive& Archive)
    {
        return TSharedPtr<LAsset>();
    }
}
