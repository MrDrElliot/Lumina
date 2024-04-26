#pragma once

#include <cstdint>
#include <string>
#include "Source/Runtime/Events/Event.h";

namespace Lumina
{
    struct FWindowSpecs
    {
        std::string Title = "Lumina";
        uint32_t Width = 1600;
        uint32_t Height = 900;

       std::function<void(FEvent&)> EventCallback;
    };
}
