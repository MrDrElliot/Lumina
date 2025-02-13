#include "Name.h"

#include "Core/Math/Hash/Hash.h"
#include "Memory/Memory.h"


namespace Lumina
{
    class FNameHashMap : public THashMap<uint64, FString>
    {
        eastl::hash_node<value_type, false> const* const* GetBuckets() const { return mpBucketArray; }
    };

    static FNameHashMap*     gNameCache = nullptr;

    void FName::Initialize()
    {
        gNameCache = FMemory::New<FNameHashMap>();
    }

    void FName::Shutdown()
    {
        FMemory::Delete(gNameCache);
    }

    FName::FName(const char* Char)
    {
        if (Char != nullptr && strlen(Char) > 0)
        {
            ID = Hash::GetHash64(Char);

            auto Itr = gNameCache->find(ID);
            if (Itr == gNameCache->end())
            {
                (*gNameCache)[ID] = FString(Char);
            }
        }
    }

    FName::FName(const FString& Str)
        :FName(Str.c_str())
    {
    }

    FName::FName(const FInlineString& Str)
        :FName(Str.c_str())
    {
    }

    const char* FName::c_str() const
    {
        if (ID == 0)
        {
            return nullptr;
        }

        auto Itr = gNameCache->find(ID);
        if (Itr != gNameCache->end())
        {
            return Itr->second.c_str();
        }

        return nullptr;
    }
}
