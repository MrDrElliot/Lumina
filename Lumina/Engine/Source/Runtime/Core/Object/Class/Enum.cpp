
#include "Core/Object/Class.h"

IMPLEMENT_INTRINSIC_CLASS(CEnum, CField, LUMINA_API)
namespace Lumina
{
    uint64 CEnum::GetEnumValueByName(FName Name)
    {
        for (const auto& Pair : Names)
        {
            if (Pair.first == Name)
            {
                return Pair.second;
            }
        }

        return INDEX_NONE;
    }

    void CEnum::AddEnum(FName FullName, uint64 Value)
    {
        const FString& FullString = FullName.ToString();

        // Find the last "::".
        size_t DelimPos = FullString.find_last_of(L':');
        if (DelimPos != std::wstring::npos && DelimPos > 0 && FullString[DelimPos - 1] == L':')
        {
            DelimPos--; // step back one for the double colon
            FString ShortName = FullString.substr(DelimPos + 2);
            Names.push_back(eastl::make_pair(FName(ShortName), Value));
        }
        else
        {
            // If no "::" found, fallback to original name
            Names.push_back(eastl::make_pair(FullName, Value));
        }
    }

    void CEnum::ForEachEnum(TFunction<void(const TPair<FName, uint64>&)> Functor)
    {
        for (const auto& Pair : Names)
        {
            Functor(Pair);
        }
    }
}