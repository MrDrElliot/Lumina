#include "AssetRegistry.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    void FAssetRegistry::ProjectLoaded()
    {
        BuildAssetDictionary();
    }

    void FAssetRegistry::Initialize(FSubsystemManager& Manager)
    {
        BuildAssetDictionary();
    }

    void FAssetRegistry::Deinitialize()
    {
    }

    void FAssetRegistry::BuildAssetDictionary()
    {
        if (bCurrentlyProcessing.exchange(true))
        {
            bHasQueuedRequest.store(true);
            return;
        }
        
        ClearAssets();
        
        TVector<FString> PackagePaths;
        for (const auto& [ID, Path] : Paths::GetMountedPaths())
        {
            FTaskSystem::Get().ScheduleLambda(1, [this, Path](uint32, uint32, uint32)
            {
                PendingTasks.fetch_add(1);
                for (const auto& Directory : std::filesystem::recursive_directory_iterator(Path.c_str()))
                {
                    if (!Directory.is_directory() && Directory.path().extension() == ".lasset")
                    {
                        FAssetData Data;
                        Data.Path = Directory.path().generic_string().c_str();
                        Data.Name = Directory.path().filename().stem().generic_string().c_str();
                    
                        FString VirtualPath = Paths::ConvertToVirtualPath(Data.Path);
                        CPackage* Package = FindObject<CPackage>(nullptr, VirtualPath);
                    
                        FString ClassPath;
                        if (Package == nullptr)
                        {
                            TVector<uint8> PackageData;
                            if (!FileHelper::LoadFileToArray(PackageData, Data.Path, 0, sizeof(FPackageHeader)))
                            {
                                LOG_ERROR("[AssetRegistry] - Invalid package found {0}", Data.Path);
                                {
                                    CorruptedAssets.emplace(Data.Path);
                                }
                                continue;
                            }
                    
                            FMemoryReader Reader(PackageData);
                            FPackageHeader Header;
                            Reader << Header;
                            ClassPath = Header.ClassPath;
                    
                            if (ClassPath.empty() || Header.Version != 1)
                            {
                                LOG_ERROR("[AssetRegistry] - Invalid package found {0}", Data.Path);
                                {
                                    CorruptedAssets.emplace(Data.Path);
                                }
                                continue;
                            }
                        }
                        else
                        {
                            ClassPath = Package->TopLevelClassName;
                        }
                    
                        Data.ClassName = ClassPath;
                        AddAsset(Data);
                    }
                }

                if (PendingTasks.fetch_sub(1) == 1)
                {
                    bCurrentlyProcessing.store(false);
                }

                if (bHasQueuedRequest.exchange(false))
                {
                    BuildAssetDictionary();
                }
            });
        }
    }

    void FAssetRegistry::GetAssets(const FARFilter& Filter, TVector<FAssetData>& OutAssets)
    {
        for (const FAssetData& Data : Assets)
        {
            bool classMatches = Filter.ClassNames.empty() || 
                eastl::find(Filter.ClassNames.begin(), Filter.ClassNames.end(), Data.ClassName) != Filter.ClassNames.end();

            bool pathMatches = Filter.Paths.empty() || 
                eastl::find(Filter.Paths.begin(), Filter.Paths.end(), Data.Path) != Filter.Paths.end();

            if (classMatches && pathMatches)
            {
                OutAssets.push_back(Data);
            }
        }
    }

    FAssetData FAssetRegistry::GetAsset(const FString& Path)
    {
        FName PathName(Path);
        if (AssetPathMap.find(PathName) != AssetPathMap.end())
        {
            return AssetPathMap[PathName];
        }

        return FAssetData();
    }

    void FAssetRegistry::AddAsset(const FAssetData& Data)
    {
        Assets.push_back(Data);
        AssetPathMap.insert_or_assign(FName(Data.Path), Data);
    }

    void FAssetRegistry::ClearAssets()
    {
        Assets.clear();
        AssetPathMap.clear();
    }
}
