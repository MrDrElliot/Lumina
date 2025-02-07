#pragma once
#include <filesystem>

#include "Assets/AssetTypes.h"
#include "Memory/RefCounted.h"



namespace Lumina
{
    class ContentBrowserWindow;

    class ContentBrowserItem : public FRefCounted
    {
    public:

        ContentBrowserItem(const std::filesystem::path& InPath);
        ~ContentBrowserItem();
        
        void OnRender(TRefPtr<ContentBrowserWindow> ContentBrowser);
        
        std::filesystem::path GetPath() { return Path; }

    private:
        
        bool bFolder = false;
        bool bWantsToRename = false;
        bool bIsRenaming = false;
        char buffer[256] = {0};
        
        std::filesystem::path Path;
    };
}
