#include "StringHash.h"

#include "Reflector/Clang/Utils.h"


namespace Lumina
{
    class FNameHashMap : public eastl::hash_map<uint64_t, eastl::string>
    {
        eastl::hash_node<value_type, false> const* const* GetBuckets() const { return mpBucketArray; }
    };

    FNameHashMap* gNameCache = nullptr;
    

    void FStringHash::Initialize()
    {
        gNameCache = new FNameHashMap();
    }

    void FStringHash::Shutdown()
    {
        delete gNameCache;
        gNameCache = nullptr;
    }

    FStringHash::FStringHash(const char* Char)
    {
        if (Char != nullptr && strlen(Char) > 0)
        {
            ID = ClangUtils::HashString(Char);

            auto Itr = gNameCache->find(ID);
            if (Itr == gNameCache->end())
            {
                (*gNameCache)[ID] = eastl::string(Char);
            }
        }
    }
    

    FStringHash::FStringHash(const eastl::string& Str)
        :FStringHash(Str.c_str())
    {
    }
    
    bool FStringHash::IsNone() const
    {
        auto Itr = gNameCache->find(ID);
        return Itr == gNameCache->end();
    }

    eastl::string FStringHash::ToString() const
    {
        return eastl::string(c_str());
    }

    const char* FStringHash::c_str() const
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
