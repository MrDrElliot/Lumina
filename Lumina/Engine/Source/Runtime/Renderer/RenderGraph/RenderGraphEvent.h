#pragma once

#include "Module/API.h"
#include "Containers/String.h"

namespace Lumina
{
    class LUMINA_API FRGEvent
    {
    public:
        FRGEvent() = default;

        // Constructor for static string
        explicit FRGEvent(const char* InEventName)
            : StaticName(InEventName)
        {}

        // Constructor with format
        template<typename... Args>
        FRGEvent(FStringView FormatString, Args&&... FormatArgs)
        {
            FormattedName = std::format(FormatString, std::forward<Args>(FormatArgs)...);
        }

        const char* Get() const
        {
            return FormattedName.empty() ? StaticName : FormattedName.c_str();
        }

        bool IsValid() const
        {
            return StaticName != nullptr || !FormattedName.empty();
        }

    private:
        const char* StaticName = nullptr;
        FString FormattedName;
    };
}
