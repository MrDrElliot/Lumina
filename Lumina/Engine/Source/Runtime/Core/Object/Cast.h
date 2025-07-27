#pragma once

#include "Object.h"
#include "Class.h"
#include "Platform/WindowsPlatform.h"


namespace Lumina
{
    template <typename From, typename To> struct TCopyQualifiersFromTo                          { typedef                To Type; };
    template <typename From, typename To> struct TCopyQualifiersFromTo<const          From, To> { typedef const          To Type; };
    template <typename From, typename To> struct TCopyQualifiersFromTo<      volatile From, To> { typedef       volatile To Type; };
    template <typename From, typename To> struct TCopyQualifiersFromTo<const volatile From, To> { typedef const volatile To Type; };

    template <typename From, typename To>
    using TCopyQualifiersFromTo_T = typename TCopyQualifiersFromTo<From, To>::Type;

    template<typename To, typename From>
    requires eastl::is_base_of_v<From, To> && (sizeof(From) > 0 && sizeof(To) > 0)
    TCopyQualifiersFromTo_T<From, To>* Cast(From* Src)
    {
        if (Src)
        {
            if (((const CObject*)Src)->IsA<To>())
            {
                return (To*)Src;
            }
        }

        return nullptr;
    }
}
