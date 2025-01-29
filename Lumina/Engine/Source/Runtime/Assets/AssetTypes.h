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
    
    inline EAssetType FileExtensionToAssetType(const FString& Extension)
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

    inline FString AssetTypeToString(EAssetType InType)
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
            Name = NAME_None.c_str();
            Guid = FGuid();
            Path = NAME_None.c_str();
            OriginPath = "";
            AssetType = EAssetType::None;
        }

        uint32 Version;
        FString Name;
        FGuid Guid;
        FString Path;
        FString OriginPath;
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
        
        FORCEINLINE bool operator==(const FAssetHandle& Other) const
        {
            return Handle == Other.Handle;
        }

        FGuid Handle;

    };
}

namespace eastl
{
    template <>
    struct hash<Lumina::FAssetHandle>
    {
        size_t operator()(const Lumina::FAssetHandle& Handle) const noexcept
        {
            if (!Handle.Handle.IsValid())
            {
                return 0; // Handle empty string or nullptr
            }
            // Use a hash function for C-strings
            return eastl::hash<Lumina::FGuid>{}(Handle.Handle);
        }
    };
}

