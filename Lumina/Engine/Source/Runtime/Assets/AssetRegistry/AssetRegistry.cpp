#include "AssetRegistry.h"

#include "Core/Object/ObjectRedirector.h"
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

            if (End == PackagePaths.size())
            {
                OnInitialDiscoveryCompleted();
            }
        });
        
    }

    void FAssetRegistry::OnInitialDiscoveryCompleted()
    {
        LOG_INFO("Asset Registry Finished Initial Discovery: Num [{}]", Assets.size());
    }

    void FAssetRegistry::AssetCreated(CObject* Asset)
    {
        CPackage* Package = Asset->GetPackage();
        FName PathName = Package->GetName();
        LUM_ASSERT(AssetPackageMap.find(PathName) == AssetPackageMap.end())

        FName AssetName = Paths::FileName(Package->GetName().ToString());
        
        FAssetData* AssetData = Memory::New<FAssetData>();
        AssetData->AssetClass = Asset->GetClass()->GetName();
        AssetData->AssetName = AssetName;
        AssetData->FullPath = Package->GetName().ToString() + "." + AssetName.ToString();
        AssetData->PackageName = Package->GetName();

        Assets.emplace(AssetData);
        AssetPackageMap.emplace(PathName, AssetData);

        FString ParentPath = Paths::Parent(AssetData->PackageName.ToString());
        if (!ParentPath.empty() && ParentPath.back() == ':')
        {
            ParentPath.append("//");
        }
        
        AssetsByPath[ParentPath].push_back(AssetData);

        GetOnAssetRegistryUpdated().Broadcast();
    }

    void FAssetRegistry::AssetDeleted(CPackage* Package)
    {
        FName PathName = Package->GetName();
        LUM_ASSERT(AssetPackageMap.find(PathName) != AssetPackageMap.end())

        FAssetData* Data = AssetPackageMap.at(PathName);
        AssetPackageMap.erase(PathName);
        Assets.erase(Data);

        FString ParentPath = Paths::Parent(PathName.ToString());
        if (!ParentPath.empty() && ParentPath.back() == ':')
        {
            ParentPath.append("//");
        }

        TVector<FAssetData*>& Paths = AssetsByPath[ParentPath];

        for (SIZE_T i = 0; i < Paths.size(); ++i)
        {
            FAssetData* Path = Paths[i];
            if (Path == Data)
            {
                Paths.erase_unsorted(Paths.begin() + i);
                break;
            }
        }

        if (Data)
        {
            Memory::Delete(Data);
        }
        
        GetOnAssetRegistryUpdated().Broadcast();
    }

    void FAssetRegistry::AssetRenamed(CObject* Asset, const FString& OldPackagePath)
    {
        CPackage* NewPackage = Asset->GetPackage();
        CPackage* OldPackage = FindObject<CPackage>(nullptr, Paths::ConvertToVirtualPath(OldPackagePath));
        
        FName NewPackagePathName = NewPackage->GetName();
        
        LUM_ASSERT(AssetPackageMap.find(NewPackage->GetName()) == AssetPackageMap.end())
        LUM_ASSERT(AssetPackageMap.find(OldPackage->GetName()) != AssetPackageMap.end())
        
        FString NewParentPath = Paths::Parent(NewPackage->GetName().ToString());

        auto MakeParentPackagePath = [] (FString& ParentPath)
        {
            if (!ParentPath.empty() && ParentPath.back() == ':')
            {
                ParentPath.append("//");
            }
        };

        MakeParentPackagePath(NewParentPath);

        FAssetData* OldData = AssetPackageMap[OldPackage->GetName()];
        OldData->AssetClass = CObjectRedirector::StaticClass()->GetFullyQualifiedName();
        
        FAssetData* Data = Memory::New<FAssetData>();
        Data->AssetName = Asset->GetName();
        Data->PackageName = NewPackage->GetName();
        Data->FullPath = Asset->GetFullyQualifiedName();
        Data->AssetClass = Asset->GetClass()->GetFullyQualifiedName();

        AssetsByPath[NewParentPath].push_back(Data);
        AssetPackageMap[NewPackage->GetName()] = Data;
        Assets.emplace(Data);

        GetOnAssetRegistryUpdated().Broadcast();
    }

    void FAssetRegistry::AssetSaved(CPackage* Package)
    {
        
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
        FString PackageFileName = Paths::FileName(VirtualPackagePath, true);
        
        FAssetData* AssetData = Memory::New<FAssetData>();

        FMemoryReader Reader(FileBinary);
        FPackageHeader Header;
        Reader << Header;
        LUM_ASSERT(Header.Tag == PACKAGE_FILE_TAG)
        
        Reader.Seek(Header.ExportTableOffset);
        
        TVector<FObjectExport> Exports;
        Reader << Exports;

        FString PackageAssetClass;
        for (const FObjectExport& ExportTable : Exports)
        {
            if (ExportTable.ObjectName == PackagePathName.ToString() + "." + PackageFileName)
            {
                PackageAssetClass = ExportTable.ClassName.ToString();
                break;
            }
        }
        
        AssetData->AssetClass = PackageAssetClass;
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
