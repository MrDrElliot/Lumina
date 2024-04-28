#include "AssetRegistry.h"

#include <fstream>

namespace Lumina
{
    bool FAssetRegistry::IsFileTypeSupported(const std::string& InExt)
    {
        for (auto Extension : SupportedExtensions)
        {
            if(Extension == InExt)
            {
                return true;
            }
        }

        return false;
    }

    void FAssetRegistry::RegisterAsset(const FAssetMetadata& InMetaData)
    {
        Registry.emplace(InMetaData.Guid, InMetaData);
        LE_LOG_INFO("Registered Asset: {0}", InMetaData.Name);
    }

    FAssetMetadata FAssetRegistry::Find(const FGuid& Guid)
    {
        auto it = Registry.find(Guid);
        if(it != Registry.end())
        {
            return it->second;
        }
        else
        {
            LE_LOG_WARN("Asset Not Found: {0}", Guid.Get());
            return {};
        }
    }
}
