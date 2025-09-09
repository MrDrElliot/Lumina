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

    void FAssetRegistry::Initialize()
    {
        
    }

    void FAssetRegistry::Deinitialize()
    {
        ClearAssets();
    }

    void FAssetRegistry::RunInitialDiscovery()
    {
        LUMINA_PROFILE_SCOPE();
        
        ClearAssets();
        
        TFixedVector<FInlineString, 256> PackagePaths;
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
        Task::AsyncTask(NumPackages, [this, PackagePaths = std::move(PackagePaths)] (uint32 Start, uint32 End, uint32)
        {
            for (uint32 i = Start; i < End; ++i)
            {
                const FInlineString& PathString = PackagePaths[i];
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
        CPackage* Package = Asset->GetPackage();
        CPackage* OldPackage = FindObject<CPackage>(nullptr, Paths::ConvertToVirtualPath(OldPackagePath));
        if (OldPackage)
        {
            FName NewPackagePathName = Package->GetName();
        
            LUM_ASSERT(AssetPackageMap.find(Package->GetName()) == AssetPackageMap.end())
            LUM_ASSERT(AssetPackageMap.find(OldPackage->GetName()) != AssetPackageMap.end())
        
            FString NewParentPath = Paths::Parent(Package->GetName().ToString());

            auto MakeParentPackagePath = [] (FString& ParentPath)
            {
                if (!ParentPath.empty() && ParentPath.back() == ':')
                {
                    ParentPath.append("//");
                }
            };

            MakeParentPackagePath(NewParentPath);

            FAssetData* OldData = AssetPackageMap[OldPackage->GetName()];
            OldData->AssetClass = CObjectRedirector::StaticClass()->GetQualifiedName();
        
            FAssetData* Data = Memory::New<FAssetData>();
            Data->AssetName = Asset->GetName();
            Data->PackageName = Package->GetName();
            Data->FullPath = Asset->GetQualifiedName();
            Data->AssetClass = Asset->GetClass()->GetQualifiedName();

            AssetsByPath[NewParentPath].push_back(Data);
            AssetPackageMap[Package->GetName()] = Data;
            Assets.emplace(Data);
        }

        GetOnAssetRegistryUpdated().Broadcast();
    }

    void FAssetRegistry::AssetSaved(CObject* Asset)
    {
        CPackage* Package = Asset->GetPackage();

        FString FullPathName = Paths::ResolveVirtualPath(Package->GetName().ToString());
        Paths::AddPackageExtension(FullPathName);
        
        TVector<uint8> FileBinary;
        if (!FileHelper::LoadFileToArray(FileBinary, FullPathName) || FileBinary.empty())
        {
            return;
        }

        FMemoryReader Reader(FileBinary);
        FPackageHeader Header;
        Reader << Header;
        LUM_ASSERT(Header.Tag == PACKAGE_FILE_TAG)

        Reader.Seek(Header.ImportTableOffset);

        TVector<FObjectImport> Imports;
        Reader << Imports;

        auto& Set = ReferenceMap[Package->GetName()];
        Set.clear();
        
        for (const FObjectImport& Import : Imports)
        {
            Set.insert(Import.Package);
            //.. Process references.
        }

        GetOnAssetRegistryUpdated().Broadcast();
    }

    const TVector<FAssetData*>& FAssetRegistry::GetAssetsForPath(const FName& Path)
    {
        return AssetsByPath[Path];
    }

    const THashSet<FName>& FAssetRegistry::GetReferences(const FName& Asset)
    {
        return ReferenceMap[Asset];
    }

    void FAssetRegistry::ProcessPackagePath(FStringView Path)
    {
        FScopeLock Lock(AssetsMutex);

        TVector<uint8> FileBinary;
        if (!FileHelper::LoadFileToArray(FileBinary, Path) || FileBinary.empty())
        {
            return;
        }

        FString VirtualPackagePath = Paths::ConvertToVirtualPath(Path.data());
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
        for (const FObjectExport& Export : Exports)
        {
            if (Export.ObjectName == PackagePathName.ToString() + "." + PackageFileName)
            {
                PackageAssetClass = Export.ClassName.ToString();
            }
        }

        AssetData->AssetClass = PackageAssetClass;
        AssetData->AssetName = Paths::FileName(VirtualPackagePath);
        AssetData->FullPath = VirtualPackagePath + "." + AssetData->AssetName.ToString();
        AssetData->PackageName = VirtualPackagePath;

        Reader.Seek(Header.ImportTableOffset);

        TVector<FObjectImport> Imports;
        Reader << Imports;

        ReferenceMap.insert(VirtualPackagePath);
        for (const FObjectImport& Import : Imports)
        {
            ReferenceMap[VirtualPackagePath].insert(Import.Package);
            //.. Process references.
        }

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
        FScopeLock Lock(AssetsMutex);

        for (FAssetData* Asset : Assets)
        {
            Memory::Delete(Asset);
        }
        
        Assets.clear();
        AssetPackageMap.clear();
        AssetsByPath.clear();
        ReferenceMap.clear();

        BroadcastRegistryUpdate();
    }

    void FAssetRegistry::BroadcastRegistryUpdate()
    {
        OnAssetRegistryUpdated.Broadcast();
    }
}
