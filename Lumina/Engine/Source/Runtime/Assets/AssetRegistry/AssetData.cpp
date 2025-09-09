#include "AssetData.h"

#include "Core/Object/Class.h"
#include "Core/Object/ObjectRedirector.h"

namespace Lumina
{
    bool FAssetData::IsRedirector() const
    {
        return AssetClass == CObjectRedirector::StaticClass()->GetQualifiedName();
    }
}
