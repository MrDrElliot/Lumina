#include "AssetRegistry.h"

#include <nlohmann/detail/meta/std_fs.hpp>
#include "nlohmann/json.hpp"
#include "Assets/Importers/AssetImporter.h"
#include "Assets/Importers/MeshImporter.h"
#include "Assets/Importers/TextureImporter.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "Renderer/Image.h"
#include <Renderer/Material.h>

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
        if (mAssetRegistry.find(InHandle) == mAssetRegistry.end())
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
        
        FArchive Ar(EArchiverFlags::Reading);
        Ar.ReadFromFile(InPath.string());
        Ar << Metadata;
        SetMetadata(FAssetHandle(Metadata.Guid), Metadata);
        return Metadata;
    }

    void AssetRegistry::SetMetadata(const FAssetHandle& InHandle, const FAssetMetadata& InMetadata)
    {
        // Update the main registry
        mAssetRegistry[InHandle] = InMetadata;

        // Retrieve the list of metadata for this asset type
        TFastVector<FAssetMetadata>& Assets = AssetTypeMap[InMetadata.AssetType];

        // Check for existing metadata with the same AssetID to prevent duplicates
        auto It = std::find_if(Assets.begin(), Assets.end(), [&](const FAssetMetadata& Meta)
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


    void AssetRegistry::GetAllRegisteredAssets(TFastVector<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(mAssetRegistry.size());
        for (auto& reg : mAssetRegistry)
        {
            OutAssets.push_back(reg.second);
        }
    }

    void AssetRegistry::GetAllAssetsOfType(EAssetType Type, TFastVector<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(100);
        if (AssetTypeMap.find(Type) != AssetTypeMap.end())
        {
            TFastVector<FAssetMetadata> Assets = AssetTypeMap.at(Type);
            for (FAssetMetadata& Meta : Assets)
            {
                if (Meta.AssetType == Type)
                {
                    OutAssets.push_back(Meta);
                }
            }
        }
    }

    FAssetHandle AssetRegistry::ImportAsset(const std::string& Name, void* Data, const std::filesystem::path& ImportFilePath, const std::filesystem::path& NewAssetPath)
    {
        AssetRegistry* AssetRegistry = AssetRegistry::Get();

        FAssetHandle NewHandle(FGuid::Generate());

        FAssetMetadata NewMetadata;
        NewMetadata.Name = Name;
        NewMetadata.Guid = NewHandle.Handle;
        NewMetadata.Path = NewAssetPath.string() + "/" + Name + FILE_EXTENSION;
        NewMetadata.OriginPath = ImportFilePath.string();
        NewMetadata.AssetType = FileExtensionToAssetType(ImportFilePath.extension().string());

        FArchive Ar(EArchiverFlags::Writing);
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
        Ar.WriteToFile(NewAssetPath / FullFileName);

        AssetRegistry->SetMetadata(NewHandle, NewMetadata);

        return NewHandle;
    }

    FAssetHandle AssetRegistry::CreateAsset(EAssetType Type, const std::string& Name, void* Data, const std::filesystem::path& NewAssetPath)
    {
        FAssetHandle NewHandle(FGuid::Generate());

        FAssetMetadata NewMetadata;
        NewMetadata.Name = Name;
        NewMetadata.Guid = NewHandle.Handle;
        NewMetadata.Path = NewAssetPath.string() + "/" + Name + FILE_EXTENSION;
        NewMetadata.OriginPath = "";
        NewMetadata.AssetType = Type;

        FArchive Ar(EArchiverFlags::Writing);

        switch (Type)
        {
            case(EAssetType::Material):
            {

                FMaterialAttributes& Attributes = *(FMaterialAttributes*)Data;

               // Ar << Attributes;

                break;
            }
        }

        std::string FullFileName = Name + FILE_EXTENSION;
        Ar.WriteToFile(NewAssetPath / FullFileName);

        return NewHandle;
    }
}
