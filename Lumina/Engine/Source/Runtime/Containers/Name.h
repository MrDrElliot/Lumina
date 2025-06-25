#pragma once

#include "String.h"
#include "Core/DisableAllWarnings.h"

PRAGMA_DISABLE_ALL_WARNINGS
#include "EASTL/hash_map.h"
#include "EASTL/hash_set.h"
PRAGMA_ENABLE_ALL_WARNINGS

#include "Module/API.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FNameHashMap;
    using FNameHashNode = eastl::hash_node<eastl::pair<const uint64, FString>, false>;
    
    class LUMINA_API FName
    {
    public:
        
        // Initialize global state.
        static void Initialize();

        // Shutdown global state.
        static void Shutdown();

    public:

        FName();
        FName(const char* Char);
        FName(const TCHAR* Char);
        explicit FName(uint64 InID) :ID(InID) {}
        explicit FName(const FString& Str);
        explicit FName(const FInlineString& Str);

        FORCEINLINE bool IsValid() const;
        bool IsNone() const { return *this != FName("NAME_None"); }
        FORCEINLINE uint64 GetID() const { return ID; }
        FORCEINLINE operator uint64() const { return ID; }

        FORCEINLINE FString ToString() const;
        
        FORCEINLINE void Clear() { ID = 0; }
        const char* c_str() const;

        FORCEINLINE bool operator==(const FName& Other) const { return ID == Other.ID; }
        FORCEINLINE bool operator!=(const FName& Other) const { return ID != Other.ID; }
    
        
    private:
        const char* StringView = nullptr;
        uint64      ID = 0;
    };
    
}

namespace eastl
{
    template <typename T> struct hash;

    template <>
    struct hash<Lumina::FName>
    {
        size_t operator()(const Lumina::FName& ID) const { return (uint64) ID; }
    };
}

namespace fmt
{
    template <>
    struct formatter<Lumina::FName>
    {
        constexpr auto parse(::fmt::format_parse_context& ctx) -> decltype(ctx.begin())
        {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const Lumina::FName& str, FormatContext& ctx) -> decltype(ctx.out())
        {
            return fmt::format_to(ctx.out(), "{}", str.c_str());
        }
    };
}