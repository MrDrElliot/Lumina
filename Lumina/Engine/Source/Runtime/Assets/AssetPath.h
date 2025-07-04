#pragma once

#include <filesystem>
#include "AssetTypes.h"
#include "Core/Assertions/Assert.h"
#include "Core/Serialization/Archiver.h"


namespace Lumina
{
    class FAssetPath
    {
    public:

        friend class FAssetRegistry;

        FAssetPath() = default;

        
        FAssetPath(const FString& InAssetPath)
            : AssetPath(InAssetPath)
        {
        }

        FAssetPath(const FString& InAssetPath, const FString InRawPath)
            : AssetPath(InAssetPath)
            , RawPath(InRawPath)
        {
        }

        FORCEINLINE FString GetPathAsString() const { return AssetPath; }
        FORCEINLINE FString GetRawPathAsString() const { return RawPath; }

        
        FORCEINLINE bool IsValid() const
        {
            return !AssetPath.empty();
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

        /** Virtualized path to an asset.. */
        FString AssetPath = {};

        /** May or may not be valid, looking to the original raw asset on disk. (E.g. ".fbx, .png")*/
        FString RawPath = {};
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

namespace fmt
{
    template <>
    struct formatter<Lumina::FAssetPath>
    {
        // Parses the format specifier (not used here, so we ignore it)
        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            return ctx.begin();
        }

        // Formats the FGuid instance
        template <typename FormatContext>
        auto format(const Lumina::FAssetPath& guid, FormatContext& ctx) -> decltype(ctx.out())
        {
            // Use FGuid's ToString method to get a string representation
            return fmt::format_to(ctx.out(), "{}", guid.GetPathAsString());
        }
    };
}
