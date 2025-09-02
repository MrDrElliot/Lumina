#pragma once

#include "String.h"
#include "Core/DisableAllWarnings.h"
#include "Core/LuminaMacros.h"
#include "Core/Math/Hash/Hash.h"

enum class EName : uint32;
PRAGMA_DISABLE_ALL_WARNINGS
#include "EASTL/hash_map.h"
#include "EASTL/hash_set.h"
PRAGMA_ENABLE_ALL_WARNINGS

#include "Module/API.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    // Pre-allocated string storage pool - grows but never shrinks
    class FStringPool
    {
    public:
        static constexpr size_t CHUNK_SIZE = 1024 * 1024; // 1MB chunks
        
        struct Chunk
        {
            alignas(64) char Data[CHUNK_SIZE];
            size_t Used = 0;
            Chunk* Next = nullptr;
        };
        
        Chunk* Head = nullptr;
        Chunk* Current = nullptr;
        
        const char* AllocateString(const char* Str, size_t Length);
        
        ~FStringPool();
    };

    class FNameTable
    {
    private:
        
        eastl::hash_map<uint64, const char*> HashToString;
        FStringPool Pool;
        
        static constexpr size_t INITIAL_CAPACITY = 16384;
        
    public:
        FNameTable()
        {
            HashToString.reserve(INITIAL_CAPACITY);
            HashToString.insert_or_assign(0, "NAME_None");
        }
        
        uint64 GetOrCreateID(const char* Str)
        {
            if (!Str || !Str[0])
            {
                return 0;
            }

            const uint64 ID = Hash::XXHash::GetHash64(Str);
            
            auto It = HashToString.find(ID);
            if ((It != HashToString.end()))
            {
                return ID;
            }
            
            const size_t Length = strlen(Str);
            const char* PermanentStr = Pool.AllocateString(Str, Length);
            
            HashToString.insert_or_assign(ID, PermanentStr);
            
            return ID;
        }
        
        const char* GetString(uint64 ID) const
        {
            auto It = HashToString.find(ID);
            return (It != HashToString.end()) ? It->second : nullptr;
        }
        
        size_t GetMemoryUsage() const
        {
            return HashToString.size() * (sizeof(uint64) + sizeof(char*)) + GetStringPoolUsage();
        }
        
    private:
        
        size_t GetStringPoolUsage() const
        {
            size_t Total = 0;
            for (FStringPool::Chunk* Chunk = Pool.Head; Chunk; Chunk = Chunk->Next)
            {
                Total += Chunk->Used;
            }
            return Total;
        }
    };

    extern LUMINA_API FNameTable* GNameTable;
    
    class LUMINA_API FName
    {
    public:

        // Helps with string API concepts.
        using value_type = char;
        
        static void Initialize();

        static void Shutdown();

    public:
        FName() = default;
        
        FName(EName) : ID(0) {}
        
        FName(const char* Str)
        {
            ID = GNameTable->GetOrCreateID(Str);
            
            #if _DEBUG
            StringView = Str;
            #endif
        }
        
        FName(const TCHAR* Str) : FName(WIDE_TO_UTF8(Str)) {}
        FName(const FString& Str) : FName(Str.c_str()) {}
        FName(const FInlineString& Str) : FName(Str.c_str()) {}
        
        explicit FName(uint64 InID)
            : ID(InID) 
        {
            #if _DEBUG
            StringView = GNameTable->GetString(ID);
            #endif
        }

        bool IsNone() const { return ID == 0; }
        uint64 GetID() const { return ID; }
        operator uint64() const { return ID; }

        const char* c_str() const
        {
            return GNameTable->GetString(ID);
        }
        
        FString ToString() const
        {
            const char* Str = c_str();
            return FString(Str);
        }

        SIZE_T Length() const
        {
            return strlen(GNameTable->GetString(ID));
        }

        FName& operator=(const EName) { ID = 0; return *this; }
        
        bool operator==(const FName& Other) const { return ID == Other.ID; }
        bool operator!=(const FName& Other) const { return ID != Other.ID; }
        bool operator<(const FName& Other) const { return ID < Other.ID; }
        
        bool operator==(const EName) const { return ID == 0; }
        bool operator!=(const EName) const { return ID != 0; }
        
        size_t hash() const { return static_cast<size_t>(ID); }

    private:
        #if _DEBUG
        FInlineString StringView = "NAME_None";
        #endif
        uint64 ID = 0;
    };
    
}

namespace eastl
{
    template <typename T> struct hash;

    template <>
    struct eastl::hash<Lumina::FName>
    {
        size_t operator()(const Lumina::FName& Name) const
        {
            return Name.hash();
        }
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