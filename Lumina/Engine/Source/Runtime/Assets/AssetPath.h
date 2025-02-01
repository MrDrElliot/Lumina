#pragma once

#include <filesystem>
#include "AssetTypes.h"
#include "Core/Serialization/Archiver.h"


namespace Lumina
{
    class FAssetPath
    {
    public:

        FAssetPath() = default;

        FAssetPath(const FString& InAssetPath)
            : AssetPath(InAssetPath)
        {}

        FAssetPath(const FString& InAssetPath, const FString InRawPath)
            : AssetPath(InAssetPath)
            , RawPath(InRawPath)
        {}

        FORCEINLINE FString GetPathAsString() const { return AssetPath; }
        
        FORCEINLINE bool IsValid() const
        {
            if (AssetPath.empty()) return false;
            
            std::filesystem::path AsPath = GetPathAsString().c_str();
            return (std::filesystem::exists(AsPath) && !std::filesystem::is_directory(AsPath));
        }

        FORCEINLINE bool operator == (const FAssetPath& Other) const { return AssetPath == Other.AssetPath; }
        FORCEINLINE bool operator != (const FAssetPath& Other) const { return AssetPath != Other.AssetPath; }


        friend FArchive& operator << (FArchive& Ar, FAssetPath& Data)
        {
            Ar << Data.AssetPath;
            Ar << Data.RawPath;

            return Ar;
        }
        
    private:

        /** Full path to an asset on disk. */
        FString AssetPath;

        /** May or may not be valid, looking to the original raw asset on disk.*/
        FString RawPath;
    };
}

namespace eastl
{
    template <>
    struct hash<Lumina::FAssetPath>
    {
        size_t operator()(const Lumina::FAssetPath& Asset) const noexcept
        {
            return eastl::hash<eastl::string_view>{}(eastl::string_view(Asset.GetPathAsString()));
        }
    };
}
