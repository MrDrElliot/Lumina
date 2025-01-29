
#include "AssetRegistry.h"
#include <nlohmann/detail/meta/std_fs.hpp>
#include "nlohmann/json.hpp"
#include "Assets/Importers/AssetImporter.h"
#include "Assets/Importers/MeshImporter.h"
#include "Assets/Importers/TextureImporter.h"
#include "Project/Project.h"
#include "Renderer/Image.h"
#include <Renderer/Material.h>
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    AssetRegistry::AssetRegistry()
    {
    }

    AssetRegistry::~AssetRegistry()
    {
        bShouldScan.store(false, std::memory_order_relaxed);
        ScanThread.join();
    }

    void AssetRegistry::StartAssetScan()
    {
        ScanThread = std::thread(&AssetRegistry::ScanAssets, this);
    }

    FAssetMetadata AssetRegistry::GetMetadata(const FAssetHandle& InHandle)
    {
        auto It = mAssetRegistry.find(InHandle);
        if (It == mAssetRegistry.end())
        {
            LOG_WARN("Failed to find asset metadata for AssetHandle: {0}", InHandle.Handle);
            return {};
        }

        return mAssetRegistry.at(InHandle);
    }

    FAssetMetadata AssetRegistry::GetMetadataByPath(const std::filesystem::path& InPath)
    {
        bool bFound = false;
        FAssetMetadata Metadata;
        for (auto& KVP : mAssetRegistry)
        {
            if(strcmp(KVP.second.Path.c_str(), InPath.string().c_str()) != 0)
            {
                bFound = true;
                Metadata = KVP.second;
            }
        }

        if(bFound)
        {
            return Metadata;
        }

        TVector<uint8> Buffer;
        FFileHelper::LoadFileToArray(Buffer, InPath);

        FMemoryReader Reader(Buffer);
        Reader << Metadata;
        
        if (strcmp(Metadata.Path.c_str(), InPath.string().c_str()) != 0)
        {
            LOG_WARN("Loaded a file that was saved with a different file path!");
            Metadata.Path = InPath.string().c_str();
        }
        
        SetMetadata(FAssetHandle(Metadata.Guid), Metadata);
        return Metadata;

    }

    void AssetRegistry::SetMetadata(const FAssetHandle& InHandle, const FAssetMetadata& InMetadata)
    {
        mAssetRegistry[InHandle] = InMetadata;

        TVector<FAssetMetadata>& Assets = AssetTypeMap[InMetadata.AssetType];

        auto It = eastl::find_if(Assets.begin(), Assets.end(), [&](const FAssetMetadata& Meta)
        {
            return Meta.Guid == InMetadata.Guid;
        });

        if (It != Assets.end())
        {
            *It = InMetadata;
        }
        else
        {
            Assets.push_back(InMetadata);
        }
    }


    void AssetRegistry::ScanAssets()
    {
        while (bShouldScan.load(std::memory_order_relaxed))
        {
            // Use recursive_directory_iterator to scan all subdirectories
            for (const auto& entry : std::filesystem::recursive_directory_iterator(Project::GetProjectContentDirectory()))
            {
                if (!entry.is_directory())
                {
                    GetMetadataByPath(entry);
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }


    void AssetRegistry::GetAllRegisteredAssets(TVector<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(mAssetRegistry.size());
        for (auto& reg : mAssetRegistry)
        {
            OutAssets.push_back(reg.second);
        }
    }

    void AssetRegistry::GetAllAssetsOfType(EAssetType Type, TVector<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(100);
        auto It = AssetTypeMap.find(Type);
        if (It != AssetTypeMap.end())
        {
            TVector<FAssetMetadata> Assets = AssetTypeMap.at(Type);
            for (FAssetMetadata& Meta : Assets)
            {
                if (Meta.AssetType == Type)
                {
                    OutAssets.push_back(Meta);
                }
            }
        }
    }

    FAssetHandle AssetRegistry::ImportAsset(const FString& Name, void* Data, const std::filesystem::path& ImportFilePath, const std::filesystem::path& NewAssetPath)
    {
        AssetRegistry* AssetRegistry = AssetRegistry::Get();
        FAssetHandle NewHandle(FGuid::Generate());

        std::filesystem::path FullPath = NewAssetPath / (Name + FILE_EXTENSION).c_str();
        
        FAssetMetadata NewMetadata;
        NewMetadata.Version = 1;
        NewMetadata.Name = Name;
        NewMetadata.Guid = NewHandle.Handle;
        NewMetadata.Path = FullPath.string().c_str();
        NewMetadata.OriginPath = ImportFilePath.string().c_str();
        NewMetadata.AssetType = FileExtensionToAssetType(ImportFilePath.extension().string().c_str());

        TVector<uint8> Buffer;
        FMemoryWriter Ar(Buffer);
        Ar << NewMetadata;
        
        switch (NewMetadata.AssetType)
        {
            case EAssetType::None:
                break;
            case EAssetType::StaticMesh:
                {
                    MeshImporter Importer;
                    Importer.Import(Ar, Data, ImportFilePath);
                    break;
                }
            case EAssetType::SkeletalMesh:
                break;
            case EAssetType::Texture:
                {   
                    TextureImporter Importer;
                    Importer.Import(Ar, Data, ImportFilePath);
                }
            case EAssetType::Material:
                break;
            case EAssetType::Prefab:
                break;
            case EAssetType::Scene:
                break;
        }

        FString FullFileName = Name + FILE_EXTENSION;
        bool bSuccess = FFileHelper::SaveArrayToFile(Buffer, NewAssetPath / FullFileName.c_str());
        if (!bSuccess)
        {
            LOG_ERROR("Failed to save array to a file when importing asset... {0}", FullFileName);
            return {};
        }
        
        AssetRegistry->SetMetadata(NewHandle, NewMetadata);

        return NewHandle;
    }

    FAssetHandle AssetRegistry::CreateAsset(EAssetType Type, const FString& Name, void* Data, const std::filesystem::path& NewAssetPath)
    {
        std::unreachable();
        return FAssetHandle();
    }
}
