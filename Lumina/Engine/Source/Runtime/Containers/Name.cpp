#include "Name.h"

#include "Core/LuminaMacros.h"
#include "Core/Math/Hash/Hash.h"
#include "Memory/Memory.h"


namespace Lumina
{
    class FNameHashMap : public THashMap<uint64, FString>
    {
        eastl::hash_node<value_type, false> const* const* GetBuckets() const { return mpBucketArray; }
    };

    FNameHashMap* GNameCache = nullptr;
    
    void FName::Initialize()
    {
        GNameCache = Memory::New<FNameHashMap>();
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

            auto Itr = GNameCache->find(ID);
            if (Itr == GNameCache->end())
            {
                (*GNameCache)[ID] = FString(Char);
            }
        }
#if _DEBUG
        StringView = (*GNameCache)[ID];
#endif
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
        return FString(c_str());
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
