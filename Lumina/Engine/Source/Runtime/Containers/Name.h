#pragma once
#include "Array.h"


namespace Lumina
{

    class FName
    {
    public:
        
        using FNameHashNode = eastl::hash_node<eastl::pair<const uint64, FString>, false>;

        // Initialize global state.
        static void Initialize();

        // Shutdown global state.
        static void Shutdown();

    public:

        FName() = default;
        explicit FName(nullptr_t) : ID(0) {}
        FName(const char* Char);
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
        
        FString     StringView;
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