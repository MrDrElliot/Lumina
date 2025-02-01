#pragma once

#include "Platform/GenericPlatform.h"
#include "Containers/String.h"


namespace Lumina
{
    class IAsset;

    enum class EAssetType : uint8
    {
        StaticMesh,
        SkeletalMesh,
        Texture,
        MaterialInstance,
        Prefab,
        Scene,
        Max,
    };

    enum class EAssetLoadState : uint8
    {
        Unloaded = 0,
        Loading,
        Loaded,
        Unloading,
        Failed,
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
        return EAssetType::Max;
    }

    inline FString AssetTypeToString(EAssetType InType)
    {
        switch (InType)
        {
            case EAssetType::StaticMesh:            return "StaticMesh";
            case EAssetType::SkeletalMesh:          return "SkeletalMesh";
            case EAssetType::Texture:               return "Texture";
            case EAssetType::MaterialInstance:      return "MaterialInstance";
            case EAssetType::Prefab:                return "Prefab";
            case EAssetType::Scene:                 return "Scene";
            case EAssetType::Max:                   return "INVALID";
        }

        return "NONE";
    }

    enum class ELoadResult
    {
        Succeeded,
        InProgress,
        Failed,
    };
}


