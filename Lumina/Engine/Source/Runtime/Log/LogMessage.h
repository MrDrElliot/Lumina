#pragma once
#include "Containers/String.h"


namespace spdlog::level
{
    enum level_enum : int;
}

namespace Lumina
{
    struct FConsoleMessage
    {
        FString Time;
        FString LoggerName;
        spdlog::level::level_enum Level;
        FString Message;
    };
}
