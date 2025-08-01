#include "AssetRegistry.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"
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
        std::unique_lock Lock(BuildMutex);
        BuildCV.wait(Lock, [this] { return !bIsBuildingAssets; });

        bIsBuildingAssets = true;
        
        ClearAssets();

        for (const auto& [ID, Path] : Paths::GetMountedPaths())
        {
            FTaskSystem::Get()->ScheduleLambda(1, [this, Path](uint32, uint32, uint32)
            {
                for (const auto& Directory : std::filesystem::recursive_directory_iterator(Path.c_str()))
                {
                    if (!Directory.is_directory() && Directory.path().extension() == ".lasset")
                    {
                        FAssetData Data;
                        Data.Path = Directory.path().generic_string().c_str();
                        Data.Name = Directory.path().filename().stem().generic_string().c_str();
                    
                        FString VirtualPath = Paths::ConvertToVirtualPath(Data.Path);
                        CPackage* Package = FindObject<CPackage>(FName(VirtualPath));
                    
                        FString ClassPath;
                        if (Package == nullptr)
                        {
                            TVector<uint8> PackageData;
                            if (!FileHelper::LoadFileToArray(PackageData, Data.Path))
                            {
                                LOG_ERROR("[AssetRegistry] - Invalid package found {0}", Data.Path);
                                {
                                    FScopeLock Lock(AssetsMutex);
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
                                    FScopeLock Lock(AssetsMutex);
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
            
                {
                    std::lock_guard Guard(BuildMutex);
                    bIsBuildingAssets = false;
                }
                BuildCV.notify_all();
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
        FScopeLock Lock(AssetsMutex);
        Assets.push_back(Data);
        AssetPathMap.insert_or_assign(FName(Data.Path), Data);
    }

    void FAssetRegistry::ClearAssets()
    {
        FScopeLock Lock(AssetsMutex);
        Assets.clear();
        AssetPathMap.clear();
    }
}
