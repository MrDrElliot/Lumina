#include "AssetRegistry.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    void FAssetRegistry::ProjectLoaded()
    {
        RunInitialDiscovery();
    }

    void FAssetRegistry::Initialize(FSubsystemManager& Manager)
    {
        
    }

    void FAssetRegistry::Deinitialize()
    {
    }

    void FAssetRegistry::RunInitialDiscovery()
    {
        LUMINA_PROFILE_SCOPE();
        
        ClearAssets();
        
        TVector<FString> PackagePaths;
        for (const auto& [ID, Path] : Paths::GetMountedPaths())
        {
            for (const auto& Directory : std::filesystem::recursive_directory_iterator(Path.c_str()))
            {
                if (!Directory.is_directory() && Directory.path().extension() == ".lasset")
                {
                    PackagePaths.push_back(Directory.path().generic_string().c_str());
                }
                else if (Directory.is_directory())
                {
                    FName VirtualPath = Paths::ConvertToVirtualPath(Directory.path().generic_string().c_str());
                    AssetsByPath.insert(VirtualPath);
                }
            }
        }


        uint32 NumPackages = (uint32)PackagePaths.size();
        FTaskSystem::Get().ScheduleLambda(NumPackages, [this, PackagePaths = std::move(PackagePaths)] (uint32 Start, uint32 End, uint32 Thread)
        {
            for (uint32 i = Start; i < End; ++i)
            {
                const FString& PathString = PackagePaths[i];
                ProcessPackagePath(PathString);
            }
        });
        
    }

    void FAssetRegistry::AddLoadedPackage(CPackage* Package)
    {
        FName PathName = Package->GetName();
        LUM_ASSERT(AssetPackageMap.find(PathName) == AssetPackageMap.end())

        FName AssetName = Paths::FileName(Package->GetName().ToString());
        
        FAssetData* AssetData = Memory::New<FAssetData>();
        AssetData->AssetClass = Package->TopLevelClassName;
        AssetData->AssetName = AssetName;
        AssetData->FullPath = Package->GetName().ToString() + "." + AssetName.ToString();
        AssetData->PackageName = Package->GetName();
        
        AssetPackageMap.emplace(PathName, AssetData);

        FString ParentPath = Paths::Parent(AssetData->PackageName.ToString());
        if (!ParentPath.empty() && ParentPath.back() == ':')
        {
            ParentPath.append("//");
        }
        
        AssetsByPath[ParentPath].push_back(AssetData);

        GetOnAssetRegistryUpdated().Broadcast();
    }

    void FAssetRegistry::RemoveLoadedPackage(CPackage* Package)
    {
    }

    FAssetRegistryUpdatedDelegate& FAssetRegistry::GetOnAssetRegistryUpdated()
    {
        return OnAssetRegistryUpdated;
    }

    const TVector<FAssetData*>& FAssetRegistry::GetAssetsForPath(const FName& Path)
    {
        return AssetsByPath[Path];
    }

    void FAssetRegistry::ProcessPackagePath(const FString& Path)
    {
        TVector<uint8> FileBinary;
        if (!FileHelper::LoadFileToArray(FileBinary, Path) || FileBinary.empty())
        {
            return;
        }

        FString VirtualPackagePath = Paths::ConvertToVirtualPath(Path);
        FName PackagePathName = VirtualPackagePath;
        
        FAssetData* AssetData = Memory::New<FAssetData>();

        FMemoryReader Reader(FileBinary);
        FPackageHeader Header;
        Reader << Header;
        LUM_ASSERT(Header.Tag == PACKAGE_FILE_TAG)
        
        Reader.Seek(Header.ExportTableOffset);
        
        TVector<FObjectExport> Exports;
        Reader << Exports;


        AssetData->AssetClass = Header.ClassPath;
        AssetData->AssetName = Paths::FileName(VirtualPackagePath);
        AssetData->FullPath = VirtualPackagePath + "." + AssetData->AssetName.ToString();
        AssetData->PackageName = VirtualPackagePath;

        FScopeLock Lock(AssetsMutex);
        Assets.emplace(AssetData);

        AssetPackageMap.emplace(PackagePathName, AssetData);

        FString ParentPath = Paths::Parent(VirtualPackagePath);
        if (!ParentPath.empty() && ParentPath.back() == ':')
        {
            ParentPath.append("//");
        }
        AssetsByPath[ParentPath].push_back(AssetData);
        
    }

    void FAssetRegistry::ClearAssets()
    {
        for (FAssetData* Asset : Assets)
        {
            Memory::Delete(Asset);
        }
        
        Assets.clear();
        AssetPackageMap.clear();
        
    }

    void FAssetRegistry::BroadcastRegistryUpdate()
    {
        OnAssetRegistryUpdated.Broadcast();
    }
}
