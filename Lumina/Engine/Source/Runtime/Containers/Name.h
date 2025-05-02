#pragma once
#include "Array.h"


namespace Lumina
{
    class FNameHashMap;
    using FNameHashNode = eastl::hash_node<eastl::pair<const uint64, FString>, false>;

    LUMINA_API extern FNameHashMap* gNameCache;

    class LUMINA_API FName
    {
    public:
        
        // Initialize global state.
        static void Initialize();

        // Shutdown global state.
        static void Shutdown();

    public:

        FName() = default;
        explicit FName(nullptr_t) : ID(0) {}
        FName(const char* Char);
        FName(const TCHAR* Char);
        explicit FName(uint64 InID) :ID(InID) {}
        explicit FName(const FString& Str);
        explicit FName(const FInlineString& Str);

        FORCEINLINE bool IsValid() const { return ID != 0; }
        bool IsNone() const;
        FORCEINLINE uint64 GetID() const { return ID; }
        FORCEINLINE operator uint64() const { return ID; }

        FORCEINLINE FString ToString() const;
        
        FORCEINLINE void Clear() { ID = 0; }
        const char* c_str() const;

        FORCEINLINE bool operator==(const FName& Other) const { return ID == Other.ID; }
        FORCEINLINE bool operator!=(const FName& Other) const { return ID != Other.ID; }
        
    private:

        #if _DEBUG
        FString     StringView;
        #endif
        
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