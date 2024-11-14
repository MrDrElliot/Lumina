#pragma once
#include <filesystem>

#include "Assets/AssetTypes.h"
#include "Memory/RefCounted.h"



namespace Lumina
{
    class ContentBrowserWindow;

    class ContentBrowserItem : public RefCounted
    {
    public:

        ContentBrowserItem(const std::filesystem::path& InPath, const FAssetMetadata& Metadata, bool bIsFolder = false);
        ~ContentBrowserItem();
        
        void OnRender(TRefPtr<ContentBrowserWindow> ContentBrowser);
        
        std::filesystem::path GetPath() { return Path; }

    private:
        
        bool bFolder = false;
        FAssetMetadata AssetMetadata;
        std::filesystem::path Path;
    };
}
