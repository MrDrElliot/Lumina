#include "AssetRegistry.h"
#include <nlohmann/json.hpp>
#include "Core/Threading/Thread.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"

namespace Lumina
{
    void FAssetRegistry::Initialize(FSubsystemManager& Manager)
    {
        UpdateAssetDictionary();
    }

    void FAssetRegistry::Deinitialize()
    {
        StopDiscoveryThread();
    }

    void FAssetRegistry::StopDiscoveryThread()
    {
        bDiscoveryThreadRunning.store(false);
        
        if (DiscoveryThread.joinable())
        {
            DiscoveryThread.join();
        }
    }

    void FAssetRegistry::UpdateAssetDictionary()
    {
        if (bDiscoveryThreadRunning.load())
        {
            return;
        }
        
        bDiscoveryThreadRunning.store(true);
        DiscoveryThread = std::thread([this]() { BeginAssetInitialDiscovery(); });
    }

    void FAssetRegistry::UpdateAssetPath(const FName& AssetName, const FString& NewPath)
    {
        if (AssetDictionary.find(AssetName) != AssetDictionary.end())
        {
            AssetDictionary[AssetName].Path = NewPath;
            LOG_INFO("Updated asset path for {} to {}", AssetName.ToString(), NewPath);
        }
        else
        {
            LOG_WARN("Asset {} not found for path update", AssetName.ToString());
        }
    }

    FAssetHeader FAssetRegistry::GetAssetHeader(const FName& AssetName)
    {
        auto It = AssetDictionary.find(AssetName);
        if (It != AssetDictionary.end())
        {
            return It->second;
        }
        
        return {};
    }

    void FAssetRegistry::BeginAssetInitialDiscovery()
    {
        Threading::InitializeThreadHeap();

        while (bDiscoveryThreadRunning.load())
        {
            FString ContentDirectory = FProject::Get()->GetProjectContentDirectory();

            if (!FProject::Get()->HasLoadedProject())
            {
                continue;
            }

            TVector<FString> DiscoveredAssets;

            for (const auto& Directory : std::filesystem::recursive_directory_iterator(ContentDirectory.c_str()))
            {
                if (Directory.is_directory()) 
                {
                    continue;
                }

                if (Directory.path().extension() == ".lasset")
                {
                    DiscoveredAssets.emplace_back(Directory.path().generic_string().c_str());
                }
            }

            BuildAssetDictionary(DiscoveredAssets);

            bDiscoveryThreadRunning.store(false);
            break;
        }

        Threading::ShutdownThreadHeap();
    }

    void FAssetRegistry::BuildAssetDictionary(const TVector<FString>& DiscoveredAssets)
    {
        AssetDictionary.clear();

        for (const FString& AssetPath : DiscoveredAssets)
        {
            TVector<uint8> AssetBinary;

            if (!FileHelper::LoadFileToArray(AssetBinary, AssetPath))
            {
                LOG_ERROR("Failed to read asset at path: {}", AssetPath);
                continue;
            }

            FMemoryReader Reader(AssetBinary);
            FAssetHeader Header;
            Reader << Header;

            FName AssetName = FName(Paths::FileName(AssetPath, true));
            AssetDictionary.insert_or_assign(AssetName, Header);

            LOG_INFO("Discovered asset: {}", AssetName.ToString());
        }
    }
}
