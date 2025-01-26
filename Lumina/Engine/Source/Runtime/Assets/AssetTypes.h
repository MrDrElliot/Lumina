#pragma once

#include <memory>

#include "Platform/GenericPlatform.h"
#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Core/Serialization/Archiver.h"
#include "GUID/GUID.h"


namespace Lumina
{
    class LAsset;

    enum class EAssetType : uint8
    {
        None,
        StaticMesh,
        SkeletalMesh,
        Texture,
        Material,
        Prefab,
        Scene,
        Max,
    };
    
    inline EAssetType FileExtensionToAssetType(const std::string& Extension)
    {
        if(Extension == ".png" || Extension == ".jpg")
        {
            return EAssetType::Texture;
        }
        if(Extension == ".gltf")
        {
            return EAssetType::StaticMesh;
        }
        return EAssetType::None;
    }

    inline std::string AssetTypeToString(EAssetType InType)
    {
        switch (InType)
        {
            case EAssetType::None:          return "NONE";
            case EAssetType::StaticMesh:    return "Static Mesh";
            case EAssetType::SkeletalMesh:  return "Skeletal Mesh";
            case EAssetType::Texture:       return "Texture";
            case EAssetType::Material:      return "Material";
            case EAssetType::Prefab:        return "Prefab";
            case EAssetType::Scene:         return "Scene";
            case EAssetType::Max:           return "NONE";
        }

        return "NONE";
    }


    struct FAssetMetadata
    {
        FAssetMetadata()
        {
            Version = 1;
            Name = NAME_None.CStr();
            Guid = FGuid();
            Path = NAME_None.CStr();
            OriginPath = "";
            AssetType = EAssetType::None;
        }

        uint32 Version;
        std::string Name;
        FGuid Guid;
        std::string Path;
        std::string OriginPath;
        EAssetType AssetType;

        friend FArchive& operator << (FArchive& Ar, FAssetMetadata& data)
        {
            Ar << data.Version;
            Ar << data.Name;
            Ar << data.Guid;
            Ar << data.Path;
            Ar << data.OriginPath;
            Ar << data.AssetType;

            return Ar;
        }
    };

    struct FAssetHandle
    {
        FAssetHandle() = default;
        FAssetHandle(const FGuid& Guid) :Handle(Guid) {}

        
        bool IsValid() const
        {
            return Handle.IsValid();
        }

        friend FArchive& operator << (FArchive& Ar, FAssetHandle& data)
        {
            Ar << data.Handle;

            return Ar;
        }
        
        bool operator==(const FAssetHandle& Other) const
        {
            return Handle == Other.Handle;
        }

        FGuid Handle;

    };
}

namespace std
{
    template <>
    struct hash<Lumina::FAssetHandle>
    {
        std::size_t operator()(const Lumina::FAssetHandle& Handle) const noexcept
        {
            if (!Handle.Handle.IsValid())
            {
                return 0; // Handle empty string or nullptr
            }
            // Use a hash function for C-strings
            return std::hash<Lumina::FGuid>{}(Handle.Handle);
        }
    };
}

