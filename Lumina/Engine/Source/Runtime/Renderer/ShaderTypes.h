#pragma once

#include <filesystem>
#include "Platform/GenericPlatform.h"
#include "Containers/Array.h"

namespace Lumina
{
    enum class EShaderStage : uint8;

    struct FShaderData
    {
        EShaderStage Stage;
        TFastVector<uint32> Binaries;
        std::filesystem::path RawPath;
    };
}
