#include "AssetRegistry.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    void FAssetRegistry::Initialize(FSubsystemManager& Manager)
    {
    }

    void FAssetRegistry::Deinitialize()
    {
    }

    void FAssetRegistry::BuildAssetDictionary()
    {
        FScopeLock Lock(Mutex);
        Assets.clear();
        
        TVector<std::filesystem::path> AllFiles;
        FString ContentDirectory = FProject::Get()->GetProjectContentDirectory();
        
        for (const auto& Directory : std::filesystem::recursive_directory_iterator(ContentDirectory.c_str()))
        {
            if (!Directory.is_directory() && Directory.path().extension() == ".lasset")
            {
                AllFiles.emplace_back(Directory.path());
            }
        }

        uint32 NumTasks = (uint32)AllFiles.size();
        FTaskSystem::Get()->ScheduleLambda(NumTasks, [this, AllFiles = std::move(AllFiles)](enki::TaskSetPartition Range, uint32 ThreadNum)
        {
            for (int i = Range.start; i < Range.end; ++i)
            {
                const std::filesystem::path& FilePath = AllFiles[i];
                
                FAssetData Data;
                Data.Path = FilePath.generic_string().c_str();
                Data.Name = FilePath.filename().stem().generic_string().c_str();
    
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
                            FScopeLock Lock(Mutex);
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
                            FScopeLock Lock(Mutex);
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
    
                // Thread-safe insertion
                {
                    FScopeLock Lock(Mutex);
                    AssetPathMap.insert_or_assign(FName(Data.Path), Data);
                    Assets.push_back(Data);
                }
            }
        });
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
}
