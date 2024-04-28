#pragma once
#include <string>
#include <vector>
#include "Source/Runtime/Log/Log.h"

#include <nlohmann/json.hpp>

#include "Source/Runtime/Assets/Asset.h"
#include "Source/Runtime/Assets/AssetManager/AssetManager.h"

using Json = nlohmann::json;


namespace Lumina
{
    class FAssetManager;
    class FGuid;
    inline std::vector<std::string> SupportedExtensions =
    {
        ".png",
        ".glb",
        ".lum",
    };
    
    class FAssetRegistry
    {
    public:

        static FAssetRegistry& Get() { return FAssetManager::GetRegistry(); }
        
        
        static bool IsFileTypeSupported(const std::string& InExt);
        void RegisterAsset(const FAssetMetadata& InMetaData);
        FAssetMetadata Find(const FGuid& Guid);

    public:
            
        std::unordered_map<uint32_t, FAssetMetadata>& GetAssets() { return Registry; }
        
    private:

        std::unordered_map<uint32_t, FAssetMetadata> Registry;

        
    };
}
