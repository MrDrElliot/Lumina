#include "AssetRegistry.h"

#include <nlohmann/detail/meta/std_fs.hpp>
#include "nlohmann/json.hpp"
#include "Assets/Importers/AssetImporter.h"
#include "Assets/Importers/MeshImporter.h"
#include "Assets/Importers/TextureImporter.h"
#include "Renderer/Image.h"

namespace Lumina
{
    AssetRegistry::AssetRegistry()
    {
        
    }

    AssetRegistry::~AssetRegistry()
    {
        
    }

    void AssetRegistry::Shutdown()
    {
        
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
        mAssetRegistry[FAssetHandle(Metadata.Guid)] = Metadata;
        return Metadata;
    }

    void AssetRegistry::SetMetadata(const FAssetHandle& InHandle, const FAssetMetadata& InMetadata)
    {
        mAssetRegistry[InHandle] = InMetadata;
    }

    void AssetRegistry::GetAllRegisteredAssets(TFastVector<FAssetMetadata>& OutAssets)
    {
        OutAssets.reserve(mAssetRegistry.size());
        for (auto& reg : mAssetRegistry)
        {
            OutAssets.push_back(reg.second);
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
}
