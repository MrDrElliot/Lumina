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

    const FAssetMetadata& AssetRegistry::GetMetadata(const FAssetHandle& InHandle)
    {
        if (!mAssetRegistry.contains(InHandle))
        {
            LOG_WARN("Failed to find asset metadata for AssetHandle: {0}", InHandle.Handle);
        }

        return mAssetRegistry.at(InHandle);
    }

    FAssetMetadata AssetRegistry::GetMetadataByPath(const std::filesystem::path& InPath)
    {
        bool bFound = false;
        FAssetMetadata Metadata;
        for (auto& KVP : mAssetRegistry)
        {
            if(KVP.second.Path == InPath)
            {
                bFound = true;
                Metadata = KVP.second;
            }
        }

        if(bFound)
        {
            return Metadata;
        }

        TArray<uint8> Buffer;
        FFileHelper::LoadFileToArray(Buffer, InPath);

        FMemoryReader Reader(Buffer);
        Reader << Metadata;

        LOG_WARN("GUID: {0}", Metadata.Guid.ToString());

        if (Metadata.Path != InPath.string())
        {
            LOG_ERROR("Loaded a file that was saved with a different file path!");
            return {};
        }
        
        SetMetadata(FAssetHandle(Metadata.Guid), Metadata);
        return Metadata;

    }

    void AssetRegistry::SetMetadata(const FAssetHandle& InHandle, const FAssetMetadata& InMetadata)
    {
        // Update the main registry
        mAssetRegistry[InHandle] = InMetadata;

        // Retrieve the list of metadata for this asset type
        TArray<FAssetMetadata>& Assets = AssetTypeMap[InMetadata.AssetType];

        // Check for existing metadata with the same AssetID to prevent duplicates
        auto It = std::ranges::find_if(Assets, [&](const FAssetMetadata& Meta)
        {
            return Meta.Guid == InMetadata.Guid;
        });

        if (It != Assets.end())
        {
            // Update the existing metadata if found
            *It = InMetadata;
        }
        else
        {
            // Add new metadata if not found
            Assets.PushBack(InMetadata);
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


    void AssetRegistry::GetAllRegisteredAssets(TArray<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(mAssetRegistry.size());
        for (auto& reg : mAssetRegistry)
        {
            OutAssets.PushBack(reg.second);
        }
    }

    void AssetRegistry::GetAllAssetsOfType(EAssetType Type, TArray<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(100);
        if (AssetTypeMap.contains(Type))
        {
            TArray<FAssetMetadata> Assets = AssetTypeMap.at(Type);
            for (FAssetMetadata& Meta : Assets)
            {
                if (Meta.AssetType == Type)
                {
                    OutAssets.PushBack(Meta);
                }
            }
        }
    }

    FAssetHandle AssetRegistry::ImportAsset(const std::string& Name, void* Data, const std::filesystem::path& ImportFilePath, const std::filesystem::path& NewAssetPath)
    {
        AssetRegistry* AssetRegistry = AssetRegistry::Get();
        FAssetHandle NewHandle(FGuid::Generate());

        FAssetMetadata NewMetadata;
        NewMetadata.Version = 1;
        NewMetadata.Name = Name;
        NewMetadata.Guid = NewHandle.Handle;
        NewMetadata.Path = (NewAssetPath / (Name + FILE_EXTENSION)).string();
        NewMetadata.OriginPath = ImportFilePath.string();
        NewMetadata.AssetType = FileExtensionToAssetType(ImportFilePath.extension().string());

        TArray<uint8> Buffer;
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

        std::string FullFileName = Name + FILE_EXTENSION;
        bool bSuccess = FFileHelper::SaveArrayToFile(Buffer, NewAssetPath / FullFileName);
        if (!bSuccess)
        {
            LOG_ERROR("Failed to save array to a file when importing asset... {0}", FullFileName);
            return {};
        }
        
        AssetRegistry->SetMetadata(NewHandle, NewMetadata);

        return NewHandle;
    }

    FAssetHandle AssetRegistry::CreateAsset(EAssetType Type, const std::string& Name, void* Data, const std::filesystem::path& NewAssetPath)
    {
        std::unreachable();
        return FAssetHandle();
    }
}
