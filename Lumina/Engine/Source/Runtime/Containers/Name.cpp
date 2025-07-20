#include "Name.h"

#include "Core/LuminaMacros.h"
#include "Core/Math/Hash/Hash.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"


namespace Lumina
{
    class FNameHashMap : public THashMap<uint64, FString>
    {
        eastl::hash_node<value_type, false> const* const* GetBuckets() const { return mpBucketArray; }
    };

    LUMINA_API FNameHashMap* GNameCache = nullptr;

    static FMutex HashMutex;
    
    void FName::Initialize()
    {
        GNameCache = Memory::New<FNameHashMap>();
        std::cout << "[Lumina] - String ID (FName) System Initialized\n";
    }

    void FName::Shutdown()
    {
        Memory::Delete(GNameCache);
        GNameCache = nullptr;
    }

    FName::FName()
        :FName(NAME_None)
    {
    }


    FName::FName(const char* Char)
    {
        if (Char != nullptr && strlen(Char) > 0)
        {
            ID = Hash::GetHash64(Char);

            (*GNameCache).try_emplace(ID, Char);

#if _DEBUG
            StringView = FString(Char);
#endif
        }
        else
        {
            *this = NAME_None;
        }
    }

    FName::FName(const TCHAR* Char)
        :FName(WIDE_TO_UTF8(Char))
    {
    }

    FName::FName(const FString& Str)
        :FName(Str.c_str())
    {
    }

    FName::FName(const FInlineString& Str)
        :FName(Str.c_str())
    {
    }

    bool FName::IsValid() const
    {
        auto Itr = GNameCache->find(ID);
        return Itr == GNameCache->end() && ID != 0;
    }


    FString FName::ToString() const
    {
        const char* Ptr = c_str();
        return Ptr ? Ptr : "NAME_None";
    }

    const char* FName::c_str() const
    {
        if (ID == 0)
        {
            return nullptr;
        }

        auto Itr = GNameCache->find(ID);
        if (Itr != GNameCache->end())
        {
            return Itr->second.c_str();
        }

        return nullptr;
    }
}
